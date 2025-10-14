
#include "StdAfx.h"
#include "FLItemBase.h"
#include "serialnumber.h"


//////////////////////////////////////////////////////////////////////////
BOOL IsUsableItem( FLItemBase* pItem )
{
	if( pItem == NULL )
	{
		return FALSE;
	}

	if( pItem->IsEmpty() == TRUE )
	{
		return FALSE;
	}

	if( pItem->GetExtra() != 0 )
	{
		return FALSE;
	}

	return TRUE;
}

BOOL IsUsingItem( FLItemBase* pItem )
{
	if( pItem == NULL )
		return FALSE;

	if( pItem->GetExtra() != 0 )
		return TRUE;
	else
		return FALSE;
}
//////////////////////////////////////////////////////////////////////////


FLItemBase::FLItemBase() 
{ 
	Init();
}

FLItemBase::FLItemBase( const FLItemBase & rhs )
{
	Init();
	*this	= rhs;
}

void FLItemBase::Serialize( CAr & ar ) const
{
	ar << m_dwObjId << m_dwObjIndex << m_dwItemId;
	ar << m_iSerialNumber;
	ar.WriteString( m_szItemText );
}

void FLItemBase::Deserialize( CAr & ar )
{
	ar >> m_dwObjId >> m_dwObjIndex >> m_dwItemId;
	ar >> m_iSerialNumber;
	ar.ReadString( m_szItemText, _countof( m_szItemText ) );
}

void	FLItemBase::Init()
{
	m_szItemText[0]		= '\0';
	m_dwItemId			= 0;
	m_dwObjId			= NULL_ID;
	m_dwObjIndex		= NULL_ID;
	m_nExtra			= 0;
	m_nCost				= 0;
	m_iSerialNumber		= 0;
#ifdef __CLIENT
	m_bControlForbidden = FALSE;
	SetTexture( NULL );	
#endif // __CLIENT
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

void FLItemBase::Empty()
{
	m_szItemText[0] = '\0';
	m_dwItemId		= 0;
	m_dwObjId		= NULL_ID;
	m_dwObjIndex	= NULL_ID;
	m_nExtra		= 0;
#ifdef __CLIENT
	SetTexture( NULL );
#endif // __CLIENT
}

PT_ITEM_SPEC FLItemBase::GetProp() 
{ 
	return	g_xSpecManager->GetSpecItem( m_dwItemId );
}

FLItemBase &	FLItemBase::operator = ( const FLItemBase & rhs )
{
	FLStrcpy( m_szItemText, _countof( m_szItemText ), rhs.m_szItemText );
	m_dwItemId				= rhs.m_dwItemId;
	m_dwObjId				= rhs.m_dwObjId;
	m_dwObjIndex			= rhs.m_dwObjIndex;
	m_iSerialNumber			= rhs.m_iSerialNumber;

#ifdef __CLIENT
	m_pTexture				= rhs.m_pTexture;
	m_bControlForbidden		= rhs.m_bControlForbidden;
#endif

	return *this;
}

//
// 아이템 가격을 계산해서 리턴한다.
// -1을 리턴하면 처리해선 안된다.
//
int	FLItemBase::GetCost( void ) const
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_dwItemId );
	if( pItemProp == NULL )
	{
		return -1;
	}
	if( pItemProp->dwCost == 0xFFFFFFFF )	
	{
		return -1;
	}

	FLItemElem *pItemElem = (FLItemElem *)this;

	int nCost;
	if( pItemElem->GetAbilityOption() )
	{		
		//INT((아이템가격+아이템가격*(0.1+(아이템강화레벨*아이템강화레벨)/18))
		nCost = (int)( pItemProp->dwCost + pItemProp->dwCost * ( 0.1f + ( pItemElem->GetAbilityOption() * pItemElem->GetAbilityOption() ) / 18.0f ) );
	}
	else
	{
		nCost = (int)pItemProp->dwCost;
	}

	return nCost;
}

//sun: 11, 길드대전 칩보상 및 칩을 통한 상점 이용
DWORD FLItemBase::GetChipCost()
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_dwItemId );
	if( pItemProp == NULL )		
	{
		return NULL_ID;
	}
	if( pItemProp->dwReferValue1 == 0xFFFFFFFF )	
	{
		return NULL_ID;
	}

	return pItemProp->dwReferValue1;
}

// 퀘스트 아이템인가?
BOOL FLItemBase::IsQuest() const
{
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_dwItemId );
	if( pItemProp == NULL )
	{
		return FALSE;
	}

	if( pItemProp->dwItemKind3 == IK3_QUEST )
	{
		if( g_xFlyffConfig->GetMainLanguage() == LANG_JAP )
		{
			if( pItemProp->dwID == ITEM_INDEX( 6237, II_SYS_SYS_QUE_REDSOCKS ) )	// 예외 
				return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

void FLItemBase::SetExtra( int nExtra )
{
	m_nExtra = nExtra;
}

int FLItemBase::GetExtra() const
{
	return m_nExtra;
}

void	FLItemBase::SetSerialNumber( void )
{
	m_iSerialNumber	= CSerialNumber::GetInstance()->Get();
}

void	FLItemBase::SetSerialNumber( const SERIALNUMBER iSerialNumber )
{
//	FLASSERT( iSerialNumber );
//	m_iSerialNumber		= ( iSerialNumber != 0 ) ? iSerialNumber : CSerialNumber::GetInstance()->Get();

	m_iSerialNumber		= iSerialNumber;
}

void	FLItemBase::SetTexture()
{
#ifdef __CLIENT
	PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( m_dwItemId );
	if ( pItemProp == NULL )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "GetProp() NULL Return ItemId:[%d]" ), m_dwItemId );
		return;
	}

	//sun: 9, 9-10차 펫
	CString strIcon	= pItemProp->szIcon;
	if( pItemProp->dwItemKind3 == IK3_EGG )
	{
		BYTE nLevel	= 0;
		if( ( (FLItemElem*)this )->m_pPet )
			nLevel	= ( (FLItemElem*)this )->m_pPet->GetLevel();
		switch( nLevel )
		{
		case PL_D:
		case PL_C:
			strIcon.Replace( ".", "_00." );
			break;
		case PL_B:
		case PL_A:
			strIcon.Replace( ".", "_01." );
			break;
		case PL_S:
			strIcon.Replace( ".", "_02." );
			break;
		}
	}
	m_pTexture = CWndBase::m_textureMng.AddTexture( g_Neuz.m_pd3dDevice, MakePath( DIR_ITEM, strIcon ), 0xffff00ff );
#endif //__CLIENT
}

#ifdef __CLIENT
void FLItemBase::SetTexture( CTexture* pTexture )
{
	m_pTexture = pTexture;
}

CTexture* FLItemBase::GetTexture() 
{
	return m_pTexture;
}
#endif // __CLIENT


//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
