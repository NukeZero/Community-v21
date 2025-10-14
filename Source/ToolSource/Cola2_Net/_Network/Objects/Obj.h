#ifndef __OBJ_H__
#define __OBJ_H__

#pragma once

#include <d3dx9math.h>
#include "Define.h"
#include "Data.h"
#include "Ar.h"
#include <string>
#include <map>
using	namespace	std;

#include "SkillInfluence.h"

#ifdef __CHINA
 #if defined(__DBSERVER) || defined(__VPW)
  #include "..\DatabaseServerC\Project.h"
  extern	CProject	prj;
 #endif	//__DBSERVER || __VPW
#else
 #if defined(__DBSERVER) || defined(__VPW)
  #include "..\DatabaseServer\Project.h"
  extern	CProject	prj;
 #endif	//__DBSERVER || __VPW
#endif

#include "UserTaskBar.h"

#if defined(__DBSERVER)
#include "dbmanager.h"
#endif	//

#include "messenger.h"

typedef	DWORD	OBJID;

#define ITYPE_ITEM      0
#define ITYPE_CARD      1
#define ITYPE_CUBE      2 
#define ITYPE_PET		3

#define	MAX_INVENTORY	42

enum 
{
	METHOD_NONE, METHOD_EXCLUDE_ITEM
};

#define MAX_SKILL_JOB	( MAX_JOB_SKILL + MAX_EXPERT_SKILL + MAX_PRO_SKILL )

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CObj
{
public:
	DWORD	m_dwType;
	DWORD	m_dwIndex;
	D3DXVECTOR3		m_vScale;
	DWORD	m_dwMotion;
	D3DXVECTOR3		m_vPos;
	D3DXVECTOR3		m_vAxis;
	FLOAT	m_fAngle;

public:
//	Constructions
	CObj();
	virtual	~CObj()		{}
//	Operations
	virtual	void	Serialize( CAr & ar );
public:
static int	m_nMethod;
static int	GetMethod( void )	{	return m_nMethod;	}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CCtrl : public CObj
{
public:
	OBJID	m_objid;

public:
//	Constructions
	CCtrl();
	virtual	~CCtrl()	{}
//	Operations
	void	SetId( OBJID objid )	{	m_objid		= objid;	}
	OBJID	GetId( void )	{	return m_objid;	}
	virtual	void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CItemBase
{
public:
	OBJID       m_dwObjId ;		// 아이템 컨테이너에서 몇번째 
	DWORD		m_dwItemId;		// IID_ 값 
	DWORD		m_dwSerialNumber;
	TCHAR       m_szItemText[ 32 ];

	DWORD		m_dwObjIndex;	// 인벤토리에서 몇번째 

//	Constructions
	CItemBase();
	virtual ~CItemBase()	{}

//	Operations
	BOOL	IsEmpty()	{	return m_dwItemId ? FALSE : TRUE;	}
	void	Empty()
	{
		m_szItemText[0] = '\0';
		m_dwItemId      = 0;
	}
	virtual	CItemBase&	operator = ( CItemBase & ib );
	virtual void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
#ifdef __PIERCING_SM_TIME
#define	MAX_PIERCING	4
typedef	struct	_PIERCINGINFO
{
	int		nPiercedSize;
	DWORD	adwItemId[MAX_PIERCING];
}	PIERCINGINFO, *PPIERCINGINFO;
#endif	// __PIERCING_SM_TIME



class CItemElem : public CItemBase
{
private:
	int		m_nAbilityOption;

public:
	enum	{	expired	= 0x01,		};
	
	BYTE	m_nRepairNumber;
	BYTE	m_byFlag;
	short	m_nItemNum; 
	int		m_nHitPoint;
	DWORD	m_idGuild;
	BYTE	m_bItemResist;				// 어느 속성 인가?
	int		m_nResistAbilityOption;		// 속성 추가 능력치 가변 옵션
	int		m_nResistSMItemId;

#ifdef __PIERCING_SM_TIME
	PIERCINGINFO	m_piercingInfo;
	BOOL	m_bCharged;					// 상용화 아이템인지 확인
	DWORD	m_dwKeepTime;				// 지속시간
#endif	// __PIERCING_SM_TIME

#ifdef __RANDOMOPTITEM0628
	int		m_nRandomOptItemId;
#endif	// __RANDOMOPTITEM0628

public:
//	Constructions
	CItemElem();
	virtual	~CItemElem()	{}

//	Operations
	int		GetOption( void )			{ return m_nAbilityOption;	}
	void	SetOption( int nOption )	{ m_nAbilityOption = nOption; }
#ifdef __FINITEITEM0705
	BOOL	IsFlag( BYTE byFlag )		{ return ( m_byFlag & byFlag ) ? TRUE : FALSE; }
#endif	// __FINITEITEM0705

	virtual	CItemElem&	operator = ( CItemElem & ie );
	virtual	void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef	struct	tagSKILL
{
	DWORD	dwSkill;
	DWORD	dwExp;
	DWORD	dwLevel;
	BOOL	bStop;
	void Init()	{};
}
SKILL, *LPSKILL;


/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

template <class T> class CItemContainer  
{
public:
	LPDWORD	m_apIndex;
	DWORD	m_dwIndexNum;
	T*	m_apItem;
	DWORD	m_dwItemMax;

public:
//	Constructions
	CItemContainer();
	CItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = 0xffffffff );
	virtual	~CItemContainer();
//	Operations
	void Clear();
	void	SetItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra = 0xffffffff );
//	T*	Add( DWORD dwItemId );

	BOOL	Add( T* pElem, BYTE* pnId = NULL, short* pnNum = NULL, BYTE* pnCount = NULL );
	BOOL	Add( DWORD dwItemId, short nNum, int nOption, DWORD dwSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, BYTE* pnId = NULL, short* pnNum = NULL, BYTE* pnCount = NULL, BOOL bCharged = FALSE, int nPiercedSize = 0, DWORD adwItemId0 = 0, DWORD adwItemId1 = 0, DWORD adwItemId2 = 0, DWORD adwItemId3 = 0, DWORD m_dwKeepTime = 0 );
#ifdef __PIERCING_SM_TIME
	BOOL	IsFull( ItemProp* pItemProp, short nNum, BOOL bCharged );
#else // __PIERCING_SM_TIME
	BOOL	IsFull( ItemProp* pItemProp, short nNum );
#endif // __PIERCING_SM_TIME

	T*	GetAt( DWORD dwIndex );
	T*	GetAtId( OBJID dwObjId );
	T*	GetItem( ItemProp * pItemProp, int nAbilityOption );
	T*  GetItem( ItemProp * pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption );
	BOOL IsEquip( DWORD dwObjId );
	void RemoveAt( DWORD dwIndex );
	void	RemoveAtId( OBJID dwObjId );

	T*	GetEquip( DWORD dwIndex );
	BOOL	UnEquip( DWORD dwIndex );
	void	DoEquip( DWORD dwSrcIndex, DWORD dwDstIndex );
	void	Copy( CItemContainer<T> & rItemContainer );
	void	Serialize( CAr & ar );
};

template <class T> CItemContainer<T>::CItemContainer()
{
	m_apItem = NULL;
	m_apIndex = NULL;
}

template <class T> CItemContainer<T>::CItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra )
{
	m_apItem = NULL;
	m_apIndex = NULL;
	
	SetItemContainer( dwItemType, dwItemMax, dwExtra );
}

template <class T> CItemContainer<T>::~CItemContainer()
{
	SAFE_DELETE_ARRAY( m_apItem );
	SAFE_DELETE_ARRAY( m_apIndex );
}

template <class T> void CItemContainer<T>::Clear()
{
	for( int i = 0; i < m_dwItemMax; i++ )
	{
		m_apItem[ i ].Empty();
		m_apItem[ i ].m_dwObjId = i;
		if( i < m_dwIndexNum )
		{
			m_apItem[ i ].m_dwObjIndex = i;
			m_apIndex[ i ] = i;
		}
		else 
		{
//			m_apItem[i].m_dwObjIndex	= NULL_ID:
			m_apIndex[ i ] = NULL_ID;
		}
	}
}

template <class T> void CItemContainer<T>::SetItemContainer( DWORD dwItemType, DWORD dwItemMax, DWORD dwExtra )
{
	m_dwIndexNum	= dwItemMax;
	m_dwItemMax		= dwItemMax;
	if( dwExtra != 0xffffffff )
		m_dwItemMax		+= dwExtra;

	SAFE_DELETE_ARRAY( m_apItem );
	SAFE_DELETE_ARRAY( m_apIndex );

	m_apItem	= new T[ m_dwItemMax ];
	m_apIndex	= new DWORD[ m_dwItemMax ];
	for( u_long i = 0; i < m_dwItemMax; i++ )
	{
		m_apItem[i].m_dwObjId = i;
		if( i < m_dwIndexNum )
		{
			m_apItem[i].m_dwObjIndex	= i;
			m_apIndex[i]	= i;
		}
		else 
			m_apIndex[i]	= NULL_ID;
	}
}


template <class T> BOOL CItemContainer<T>::Add( DWORD dwItemId, short nNum, int nOption, DWORD dwSerialNumber, int nItemResist, int nResistAbilityOption, BYTE* pnId, short* pnNum, BYTE* pnCount, BOOL bCharged, int nPiercedSize, DWORD adwItemId0, DWORD adwItemId1, DWORD adwItemId2, DWORD adwItemId3, DWORD m_dwKeepTime )
{
#if defined(__DBSERVER)
	if( pnId )
		*pnCount	= 0;
	ItemProp* pItemProp		= prj.GetItemProp( dwItemId );
	ASSERT( pItemProp );
	BOOL bPackItem = TRUE;
	int nId;
	short nNumtmp	= nNum;

#ifdef __PIERCING_SM_TIME
	if( IsFull( pItemProp, nNum, bCharged ) )
#else // __PIERCING_SM_TIME
	if( IsFull( pItemProp, nNum ) )
#endif // __PIERCING_SM_TIME
		return FALSE;

	if( pItemProp->dwPackMax == 1 )
		bPackItem = FALSE;

	CItemElem* pElemtmp;
	if( bPackItem )
	{
		for( int i = 0; i < m_dwIndexNum; i++ )	// 합쳐질수 있으면 합치기
		{
			nId	= m_apIndex[i];
			pElemtmp	= (CItemElem*)&m_apItem[nId];
				
#ifdef __PIERCING_SM_TIME
			if( pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax && pElemtmp->m_bCharged == bCharged )
#else // __PIERCING_SM_TIME
			if( pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax )
#endif // __PIERCING_SM_TIME
			{
				if( pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax )
				{
					nNumtmp	-= ( (short)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;

					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else {
					pElemtmp->m_nItemNum	+= nNumtmp;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]		= pElemtmp->m_nItemNum;	
						( *pnCount )++;
					}
					nNumtmp = 0;
					break;
				}
			}
		}
	}

	if( nNumtmp > 0 )
	{
		for( int i = 0; i < m_dwIndexNum; i++ )
		{
			nId	= m_apIndex[i];
			pElemtmp	= (CItemElem*)&m_apItem[nId];
			if( pElemtmp->IsEmpty() )
			{
				pElemtmp->m_dwItemId	= dwItemId;
				pElemtmp->m_dwObjId		= nId;
				pElemtmp->m_dwObjIndex	= i;
				ItemProp* pItemProp		= prj.GetItemProp( dwItemId );
				if( pItemProp )
					pElemtmp->m_nHitPoint	= pItemProp->dwEndurance;
				pElemtmp->SetOption( nOption );
				pElemtmp->m_dwSerialNumber	= dwSerialNumber;
				pElemtmp->m_bItemResist = nItemResist;
				pElemtmp->m_nResistAbilityOption = nResistAbilityOption;
#ifdef __PIERCING_SM_TIME
				pElemtmp->m_bCharged = bCharged;
				pElemtmp->m_dwKeepTime	= 0;
#endif // __PIERCING_SM_TIME

#ifdef __SENDITEM_TBL_P_K
				pElemtmp->m_piercingInfo.nPiercedSize = nPiercedSize;
				pElemtmp->m_piercingInfo.adwItemId[0] = adwItemId0;
				pElemtmp->m_piercingInfo.adwItemId[1] = adwItemId1;
				pElemtmp->m_piercingInfo.adwItemId[2] = adwItemId2;
				pElemtmp->m_piercingInfo.adwItemId[3] = adwItemId3;
				if( m_dwKeepTime )
					pElemtmp->m_dwKeepTime	= m_dwKeepTime;
				
#endif // __SENDITEM_TBL_P_K
				//				_tcscpy( pElemtmp->m_szItemText, pItemProp->szName );
				
				if( nNumtmp > (short)pItemProp->dwPackMax )
				{
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;
					nNumtmp	-= (short)pItemProp->dwPackMax;

					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else
				{
					pElemtmp->m_nItemNum	= nNumtmp;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
					break;
				}
			}
		}
	}
	return TRUE;
#endif	// __DBSERVER
	return FALSE;
}

template <class T> BOOL CItemContainer<T>::Add( T* pElem, BYTE* pnId, short* pnNum, BYTE* pnCount )
{
	if( pnId )
		*pnCount	= 0;
	ItemProp* pItemProp		= prj.GetItemProp( pElem->m_dwItemId );
	if( !pItemProp )
		return FALSE;
	BOOL bPackItem = TRUE;
	int nId;

#ifdef __PIERCING_SM_TIME
//		if( prj.m_nLanguage != LANG_JAP && prj.m_nLanguage != LANG_KOR )
//			( (CItemElem*)pElem )->m_bCharged = pItemProp->bCharged;
#endif // __PIERCING_SM_TIME
	short nNumtmp	= ( (CItemElem*)pElem )->m_nItemNum;
	if( IsFull( pItemProp, nNumtmp, pElem->m_bCharged ) )	// 아이템을 넣을수 있는지 검사
		return FALSE;

	if( pItemProp->dwPackMax == 1 )
		bPackItem = FALSE;

	CItemElem* pElemtmp;
	if( bPackItem )
	{
		for( int i = 0; i < m_dwIndexNum; i++ )
		{
			nId	= m_apIndex[i];
			if( nId < 0 || nId >= m_dwItemMax )
				continue;
			pElemtmp	= (CItemElem*)&m_apItem[nId];
			
			if( pElemtmp->IsEmpty() == FALSE && pElemtmp->m_dwItemId == pElem->m_dwItemId && pElemtmp->m_nItemNum < (short)pItemProp->dwPackMax )
			{
				if( pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax )
				{
					nNumtmp	-= ( (short)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;
					if( pnId )
					{
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else {
					pElemtmp->m_nItemNum	+= nNumtmp;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]		= pElemtmp->m_nItemNum;	
						( *pnCount )++;
					}
					nNumtmp = 0;
					break;
				}
			}
			
		}
	}

	if( nNumtmp > 0 )
	{
		for( int i = 0 ; i < m_dwIndexNum ; i++ )
		{
			nId	= m_apIndex[i];
			if( nId < 0 || nId >= m_dwItemMax )
				continue;
			pElemtmp	= (CItemElem*)&m_apItem[nId];
			if( pElemtmp->IsEmpty() )	// 빈공간인지 검사
			{
				*pElemtmp	= *( (CItemElem*)pElem );
				pElemtmp->m_dwObjId	= nId;
				pElemtmp->m_dwObjIndex		= i;
//					pElemtmp->SetTexture();


#ifdef __PIERCING_SM_TIME
				memcpy( &pElemtmp->m_piercingInfo, &pElem->m_piercingInfo, sizeof(PIERCINGINFO) );
				pElemtmp->m_bCharged	= pElem->m_bCharged;
#endif	// __PIERCING_SM_TIME

#ifdef __RANDOMOPTITEM0628
				pElemtmp->m_nRandomOptItemId	= pElem->m_nRandomOptItemId;
#endif	// __RANDOMOPTITEM0628
				
				if( nNumtmp > (short)pItemProp->dwPackMax )
				{
					pElemtmp->m_nItemNum	= (short)pItemProp->dwPackMax;
					nNumtmp		-= (short)pItemProp->dwPackMax;
					if( pnId ) {
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]	= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
				}
				else
				{
					pElemtmp->m_nItemNum	= nNumtmp;
					if( pnId )
					{
						pnId[*pnCount]	= nId;
						pnNum[*pnCount]		= pElemtmp->m_nItemNum;
						( *pnCount )++;
					}
					break;
				}
			}
		}
	}

	return TRUE;
}


#ifdef __PIERCING_SM_TIME
template <class T> BOOL CItemContainer<T>::IsFull( ItemProp* pItemProp, short nNum, BOOL bCharged )
#else // __PIERCING_SM_TIME
template <class T> BOOL CItemContainer<T>::IsFull( ItemProp* pItemProp, short nNum )
#endif // __PIERCING_SM_TIME

{
	int nId;
	short nNumtmp	= nNum;

	CItemElem* pElemtmp;
	for( int i = 0; i < m_dwIndexNum; i++ )
	{
		nId	= m_apIndex[i];
		pElemtmp	= (CItemElem*)&m_apItem[nId];
		if( pElemtmp->IsEmpty() )
		{
			if( nNumtmp > (short)pItemProp->dwPackMax )
				nNumtmp	-= (short)pItemProp->dwPackMax;
			else
				return FALSE;
		}
#ifdef __PIERCING_SM_TIME
		else if( pElemtmp->m_dwItemId == pItemProp->dwID && pElemtmp->m_bCharged == bCharged )
#else // __PIERCING_SM_TIME
		else if( pElemtmp->m_dwItemId == pItemProp->dwID )
#endif // __PIERCING_SM_TIME
		{
			if( pElemtmp->m_nItemNum + nNumtmp > (short)pItemProp->dwPackMax )
				nNumtmp	-= ( (short)pItemProp->dwPackMax - pElemtmp->m_nItemNum );
			else
				return FALSE;
		}
	}

	return TRUE;
}

template <class T> T* CItemContainer<T>::GetAt( DWORD dwIndex )
{
	DWORD dwIdx		= m_apIndex[dwIndex];
	if( dwIdx == NULL_ID )
		return NULL;
	T* pItemElem	= &m_apItem[dwIdx];
	if( pItemElem->IsEmpty() ) 
		return NULL;
	return pItemElem;
}

template <class T> T* CItemContainer<T>::GetAtId( OBJID dwObjId )
{
	if( dwObjId >= m_dwItemMax )	return NULL;
	T* pItemElem	= &m_apItem[dwObjId];
	if( pItemElem->IsEmpty() )
		return NULL;
	return pItemElem;
}

template <class T> T* CItemContainer<T>::GetItem( ItemProp * pItemProp, int nAbilityOption )
{
	int nId;
	for( int i = 0 ; i < m_dwItemMax ; i++ )
	{
		nId		= m_apIndex[i];
		if( m_apItem[nId].IsEmpty() == FALSE )
		{
			T* pItemElem = &m_apItem[nId];
			if( pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption )
			{
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> T* CItemContainer<T>::GetItem( ItemProp * pItemProp, int nAbilityOption, int nItemResist, int nResistAbilityOption )
{
	int nId;
	for( int i = 0 ; i < m_dwItemMax ; i++ )
	{
		nId		= m_apIndex[i];
		if( m_apItem[nId].IsEmpty() == FALSE )
		{
			T* pItemElem = &m_apItem[nId];
			if( pItemElem->m_dwItemId == pItemProp->dwID && pItemElem->GetOption() == nAbilityOption && pItemElem->m_bItemResist == nItemResist && pItemElem->m_nResistAbilityOption == nResistAbilityOption )
			{
				return pItemElem;
			}
		}
	}
	return NULL;
}

template <class T> BOOL CItemContainer<T>::IsEquip( DWORD dwObjId )
{
	if( m_apItem[ dwObjId ].m_dwObjIndex >= m_dwIndexNum )
		return TRUE;
	return FALSE;
}
template <class T> void CItemContainer<T>::RemoveAt( DWORD dwIndex )
{
	m_apItem[ m_apIndex[ dwIndex ] ].Empty();
	if( dwIndex >= m_dwIndexNum )
	{
		m_apItem[ m_apIndex[ dwIndex ] ].m_dwObjIndex = NULL_ID;
		m_apIndex[ dwIndex ] = NULL_ID;
	}
	//for( int i = dwIndex; i < m_dwIndexNum - 1; i++ )
	//	m_apIndex[ i ] = m_apIndex[ i + 1 ];
	//m_dwIndexNum--;
}

template <class T> void CItemContainer<T>::RemoveAtId( OBJID dwObjId )
{
	if( dwObjId >= m_dwItemMax )
		return;
	if( m_apItem[ dwObjId ].m_dwObjIndex >= m_dwItemMax )
		return;
	m_apItem[ dwObjId ].Empty();
	if( m_apItem[ dwObjId ].m_dwObjIndex >= m_dwIndexNum )
	{
		m_apIndex[ m_apItem[ dwObjId ].m_dwObjIndex ] = NULL_ID;
		m_apItem[ dwObjId ].m_dwObjIndex = NULL_ID;
	}
}

template <class T> T* CItemContainer<T>::GetEquip( DWORD dwIndex )
{
	return GetAt( m_dwIndexNum + dwIndex );
}

template <class T> BOOL CItemContainer<T>::UnEquip( DWORD dwIndex )
{ 
	dwIndex		+= m_dwIndexNum;
	if( dwIndex >= m_dwItemMax )
		return FALSE;

	DWORD dwId	= m_apIndex[dwIndex];

	if( dwId >= m_dwItemMax )
		return FALSE;

	for( int i = 0; i < m_dwIndexNum; i++ )
	{
		if( m_apIndex[i] >= m_dwItemMax )
			continue;

		if( m_apItem[m_apIndex[i]].IsEmpty() )
		{
			m_apItem[m_apIndex[i]].m_dwObjIndex		= NULL_ID;
			m_apIndex[dwIndex]	= NULL_ID;
			m_apItem[dwId].m_dwObjIndex		= i;
			m_apIndex[i]	= dwId;
			return TRUE;
		}
	}
	return FALSE;
}

template <class T> void CItemContainer<T>::DoEquip( DWORD dwSrcIndex, DWORD dwDstIndex )
{
	dwDstIndex	+= m_dwIndexNum;
	for( int i = 0; i < m_dwItemMax; i++ )
	{
		if( m_apItem[i].IsEmpty() && m_apItem[i].m_dwObjIndex == NULL_ID )
		{
			m_apIndex[dwDstIndex]	= m_apIndex[dwSrcIndex];
			m_apIndex[dwSrcIndex]	= i;
			m_apItem[m_apIndex[dwSrcIndex]].m_dwObjIndex	= dwSrcIndex;
			m_apItem[m_apIndex[dwDstIndex]].m_dwObjIndex	= dwDstIndex;
			return;
		}
	}
}

template <class T> void CItemContainer<T>::Copy( CItemContainer<T> & rItemContainer )
{
	m_dwIndexNum	= rItemContainer.m_dwIndexNum;
//	memcpy( (void*)m_apIndex, (void*)rItemContainer.m_apIndex, sizeof(DWORD) * m_dwIndexNum );
	memcpy( (void*)m_apIndex, (void*)rItemContainer.m_apIndex, sizeof(DWORD) * m_dwItemMax );
	for( u_long i = 0; i < m_dwItemMax; i++ )
	{
		if( rItemContainer.m_apItem[i].IsEmpty() == FALSE )
			m_apItem[i]		= rItemContainer.m_apItem[i];
		else
			m_apItem[i].Empty();
	}
}

template <class T> void CItemContainer<T>::Serialize( CAr & ar )
{
//	for( int i = 0; i < m_dwItemMax; i++ )	// 462
//		m_apItem[i].Serialize( ar );
	DWORD	adwObjIndex[128];

	unsigned char chSize	= 0;
	
	if( ar.IsStoring() )
	{
		
		ar.Write( m_apIndex, sizeof(DWORD) * m_dwItemMax );
		u_long uOffset	= ar.GetOffset();
		ar << chSize;

		for( u_char ch = 0; ch < m_dwItemMax; ch++ )	// 0-504
		{	
			if( m_apItem[ch].IsEmpty() == FALSE )
			{
				ar << ch;
				m_apItem[ch].Serialize( ar );
				chSize++;
			}
			adwObjIndex[ch]		= m_apItem[ch].m_dwObjIndex;
		}

		ar.Write( adwObjIndex, sizeof(DWORD) * m_dwItemMax );

		int nBufSize;
		LPBYTE lpBuf	= ar.GetBuffer( &nBufSize );
		*( lpBuf + uOffset )	= chSize;
	}
	else
	{
		ar.Read( m_apIndex, sizeof(DWORD) * m_dwItemMax );
		// Clear
		for( u_int i = 0; i < m_dwItemMax; i++ )
			m_apItem[i].Empty();

		ar >> chSize;

		unsigned char ch;
		for( i = 0; i < chSize; i++ )
		{
			ar >> ch;
			m_apItem[ch].Serialize( ar );
		}

		ar.Read( adwObjIndex, sizeof(DWORD) * m_dwItemMax );
		for( i = 0; i < m_dwItemMax; i++ )
		{
			m_apItem[i].m_dwObjIndex	= adwObjIndex[i];
		}
	}
}

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CItem : public CCtrl
{
public:
	CItemBase* m_pItemBase;

//	Constructions
	CItem();
	virtual ~CItem();
//	Operations
	virtual void	Serialize( CAr & ar );
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

class CAction
{
public:
	DWORD	m_dwState;
	DWORD	m_dwStateFlag;

public:
	CAction()
		{
			m_dwState	= 0;
			m_dwStateFlag		= 0;
		}
	void	ClearStateFlag() { m_dwStateFlag = 0; }
	void	AddStateFlag( DWORD dwStateFlag ) { m_dwStateFlag |= dwStateFlag; }
	DWORD	GetStateFlag() { return m_dwStateFlag; }
	virtual	~CAction()	{}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
class CMover;
class CActionMover : public CAction
{
public:
	CActionMover()	{}
	CActionMover( CMover* pMover )	{}
	virtual	~CActionMover()	{}
	DWORD	GetState( void )	{	return m_dwState;	}
	void	__ForceSetState( DWORD dwState )	{	m_dwState	= dwState;	}
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/
typedef struct tagQuest
{
	BYTE		m_nState;
	WORD		m_wTime;
	WORD		m_wId;
	// 추가 
#ifdef __V050322_CACHEQUEST
	BYTE		m_nKillNPCNum[ 2 ];
	BYTE		m_bPatrol   : 1;
	BYTE		m_bReserve2 : 1;
	BYTE		m_bReserve3 : 1;
	BYTE		m_bReserve4 : 1;
	BYTE		m_bReserve5 : 1;
	BYTE		m_bReserve6 : 1;
	BYTE		m_bReserve7 : 1;
	BYTE		m_bReserve8 : 1;
#endif 
} QUEST,* LPQUEST;

//#ifdef __DBSERVER
#if defined(__DBSERVER)
typedef		MemPooler<CMover>	CMoverPool;
#endif	// __DBSERVER

#ifdef __FINITEITEM0705
typedef	struct	_EQUIP_INFO
{
	DWORD	dwId;
	int		nOption;
	BYTE	byFlag;
}	EQUIP_INFO,	*PEQUIP_INFO;

#endif	// __FINITEITEM0705

class CMover: public CCtrl
{
private:
	DWORD			m_dwGold;

public:
/// 몬스터에 해당되는 것 
	TCHAR			m_szCharacterKey[ 32 ];
	BOOL			m_bActiveAttack;
	DWORD			m_dwBelligerence;

	#ifdef __XFLYAI0524
		int			m_nMovePattern;
		int			m_nMoveEvent;
		int			m_nMoveEventCnt;
	#endif
/// 몬스터에 해당되는 것 

	TCHAR			m_szName[32];
	BOOL			m_bPlayer;
	u_long			m_idPlayer;
	LONG			m_nHitPoint;
	LONG			m_nManaPoint;
	LONG			m_nFatiguePoint;
	DWORD			m_dwSkinSet;
	DWORD			m_dwHairMesh;
	DWORD			m_dwHairColor;
	DWORD			m_dwHeadMesh;
	LONG			m_nFlightLv, m_nFxp;
	BYTE			m_bySex;
	DWORD			m_dwRideItemIdx;
	int				m_nFuel;
	LONG			m_nJob;
	BYTE			m_nSlot;
	char			m_szBankPass[5];
	int				m_idGuildCloak;

	LONG			m_nStr,	m_nSta,	m_nDex,	m_nInt;
	LONG			m_nLevel, m_nDeathLevel;
	EXPINTEGER		m_nExp1, m_nExp2, m_nDeathExp;
	SKILL			m_aJobSkill[ MAX_SKILL_JOB ];
	DWORD			m_aJobLv[MAX_JOB];
	OBJID			m_idMarkingWorld;
	D3DXVECTOR3		m_vMarkingPos;
	long			m_nRemainGP;
	long			m_nRemainLP;
	DWORD			m_dwAuthorization;
	DWORD			m_dwMode;

	CUserTaskBar	m_UserTaskBar;
	CMessenger		m_Messenger;

	u_long			m_idparty;
	u_long			m_idGuild;
	u_long			m_idWar;
	int				m_nNumKill;
	int				m_nSlaughter;
	int				m_nFame;
	u_long			m_idMurderer;
	int				m_nDuel;			// 듀얼중인가.( 0:없음 1:개인듀얼중 2:파티듀얼중.
	u_long			m_idDuelParty;		// 파티듀얼중이면 상대방 파티의 id
#ifdef __GUILDCOMBAT
	int				m_nGuildCombatState;
#endif // __GUILDCOMBAT

	CActionMover*					m_pActMover; 
	CItemContainer< CItemElem  >	m_Inventory ;

	u_long							m_idPlayerBank[3];
	CItemContainer< CItemElem  >	m_Bank[ 3 ] ;
	DWORD							m_dwGoldBank[ 3 ];

	D3DXVECTOR3		m_vReturnPos;						//'귀환 두루마리'관련 
	DWORD			m_dwReturnWorldID;

#ifdef __XACCFUEL0708
	DWORD			m_tmAccFuel;
#endif

#ifdef __XOPT0617
	int				m_nWeaponOption;
#else	
	#ifndef __FINITEITEM0705
	int				m_anEquipOption[ MAX_HUMAN_PARTS ];
	#endif	// __FINITEITEM0705
#endif	// __XOPT0617

	DWORD			m_dwSkillPoint;
	
#ifdef __DBSERVER
	DWORD			m_dwFlyTime;
	DWORD			m_dwWorldID;
	CMclCritSec		m_AccessLock;
#endif // __DBSERVER
	
	LONG			m_nPlusMaxHitPoint;
	DWORD			m_dwSMTime[SM_MAX];
	BYTE			m_nAttackResistLeft;
	BYTE			m_nAttackResistRight;
	BYTE			m_nDefenseResist;

	CSkillInfluence m_SkillState;				// 지속성을 갖는 스킬의 상태 처리(예:프로텍션 ??초동안 방어력 얼마 증가)

#ifdef __FINITEITEM0705
	EQUIP_INFO		m_aEquipInfo[MAX_HUMAN_PARTS];
	void	ClearEquipInfo( void )
	{
		memset( m_aEquipInfo, 0, sizeof(EQUIP_INFO) * MAX_HUMAN_PARTS );
		for( int i = 0; i < MAX_HUMAN_PARTS; i++ )
			m_aEquipInfo[i].dwId	= NULL_ID;
	}
#else	// __FINITEITEM0705
	DWORD			m_adwEquipment[ MAX_HUMAN_PARTS ];
#endif	// __FINITEITEM0705
	
#ifdef __S0504_FLY
	DWORD			m_dwStateMode;
	OBJID			m_dwUseItemId;					// 비행 시전 시간
#endif // __S0504_FLY

#ifdef __V050322_CACHEQUEST
	LPQUEST			m_aQuest;
	LPWORD			m_aCompleteQuest; 
	BYTE			m_nQuestSize;
	BYTE			m_nCompleteQuestSize;
#else // __V050322_CACHEQUEST
//{{AFX
	QUEST			m_aQuest[MAX_QUEST];
	BYTE			m_nQuestSize;
//}}AFX
#endif

#ifdef __PERIODIC_0513
	DWORD			m_dwPeriodicTick;
#endif	// __PERIODIC_0513

#ifdef __CHEER0615
	DWORD			m_dwTickCheer;
	int				m_nCheerPoint;
#endif	// __CHEER0615

public:
//	Constructions
	CMover();
	virtual	~CMover();

//	Operations
	void	InitProp( void );
	int		InitSkillExp( void );
#if defined(__DBSERVER)
	MoverProp*	GetProp( void )		{	return( prj.GetMoverProp( m_dwIndex ) );	}
#endif	// __DBSERVER

	BOOL	AddItem( DWORD dwItemType, DWORD dwId, DWORD dwNum, int nOption, DWORD dwSerialNumber, int nItemResist = 0, int nResistAbilityOption = 0, int nCharged = 0, int nPiercedSize = 0, DWORD adwItemId0 = 0, DWORD adwItemId1 = 0, DWORD adwItemId2 = 0, DWORD adwItemId3 = 0, DWORD m_dwKeepTime = 0 );
	BOOL	AddItemBank( int nSlot, DWORD dwId, DWORD dwNum, int nOption, DWORD dwSerialNumber );
	virtual void	Serialize( CAr & ar );
//	int		GetMaxHitPoint()	    { return( ( m_nLevel * 16 ) + ( m_nSta * 6 ) + ( m_nStr  * 3 ) );		}
//	int		GetMaxManaPoint()		{ return( ( m_nLevel + m_nInt ) * 2 + 20 );		}
//	int		GetMaxFatiguePoint()	{ return(int)( ( m_nLevel * 2 ) + ( m_nSta * m_nLevel / 40 ) + ( ( m_nStr + m_nSta + m_nDex ) * 2.5 ) ); }
	BOOL	IsAbnormalPlayerData();
	BOOL	NormalizePlayerData();
	void	RemoveItemIK3();
	BYTE	GetSex() { return m_bySex; }
	void	SetSex( BYTE bySex ) { m_bySex = bySex; }
	int		GetGold();
	void	SetGold( int nGold );
	BOOL	AddGold( int nGold, BOOL bSend = TRUE );

#if defined(__DBSERVER)
	void	Lock( void )	{	m_AccessLock.Enter();	}
	void	Unlock( void )	{	m_AccessLock.Leave();	}

public:
	static CMoverPool*	m_pPool;
	void*	operator new( size_t nSize )	{	return CMover::m_pPool->Alloc();	}
	void*	operator new( size_t nSize, LPCSTR lpszFileName, int nLine )	{	return CMover::m_pPool->Alloc();	}
	void	operator delete( void* lpMem )	{	CMover::m_pPool->Free( (CMover*)lpMem );	}
	void	operator delete( void* lpMem, LPCSTR lpszFileName, int nLine )	{	CMover::m_pPool->Free( (CMover*)lpMem );	}
#endif	// __DBSERVER
};

/*------------------------------------------------------------------------------------------------------------------------------------------------------------*/

inline int CMover::GetGold()
{
	ASSERT( m_dwGold <= INT_MAX );
	return m_dwGold;
}

inline void CMover::SetGold( int nGold )
{
	ASSERT( nGold >= 0 );
	m_dwGold = (DWORD)nGold;
}

inline BOOL CMover::AddGold( int nGold, BOOL bSend )
{
	if( nGold == 0 )
		return TRUE;

	int nTotal = GetGold() + nGold;

	if( nGold > 0 )
	{
		if( nTotal < 0 )		// overflow?
			nTotal = INT_MAX;
	}
	else
	{
		if( nTotal < 0 )		// underflow?
			return FALSE;
	}

	SetGold( nTotal );

	if( bSend )
	{
	#ifdef __WORLDSERVER
		g_UserMng.AddSetPointParam( this, DST_GOLD, nTotal );
	#endif	// __WORLDSERVER
	}

	return TRUE;
}

#endif	// __OBJ_H__