// UltimateWeapon.cpp: implementation of the CUltimateWeapon class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "UltimateWeapon.h"
#ifdef __WORLDSERVER
#include "../WorldServer/User.h"
#endif // __WORLDSERVER


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

//sun: 9,10차 제련 __ULTIMATE
CUltimateWeapon::CUltimateWeapon()
{
#ifdef __WORLDSERVER
	m_nSetGemProb = 0;
	m_nRemoveGemProb = 0;
	m_nGen2UniProb = 0;
	m_nUni2UltiProb= 0;
#endif // __WORLDSERVER
}

CUltimateWeapon::~CUltimateWeapon()
{

}

BOOL CUltimateWeapon::Load_GemAbility()
{
	CScanner s;
	
	if( !s.Load( "Ultimate_GemAbility.txt" ) )
		return FALSE;
	
//	int nMaxGemNum = 0;
	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "ABILITY" ) )
		{
			__GEMABILITYKIND GemAbilityKind;
			s.GetToken();
			GemAbilityKind.nAbility = CScript::GetDefineNum( s.Token );
			
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				__GEMABILITY GemAbility;
				GemAbility.dwGemItemId = CScript::GetDefineNum( s.Token );
				s.GetToken();
				GemAbility.dwAbilityItemId = CScript::GetDefineNum( s.Token );
				for( int i=0; i<5; i++ )
					GemAbility.vecAbility.push_back( s.GetNumber() );
				GemAbilityKind.vecAbilityKind.push_back( GemAbility );
				s.GetToken();
			}
			m_vecGemAbilityKind.push_back( GemAbilityKind );
		}
		else if( s.Token == _T( "LEVEL_GEM" ) )
		{
			__MAKEGEMITEM	MakeGemItem;
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				MakeGemItem.dwItemID = CScript::GetDefineNum( s.Token );
				if( MakeGemItem.dwItemID == (DWORD)-1 )		// cr
				{
					FLERROR_LOG( PROGRAM_NAME, _T( "아이템이 없는것을 설정했음" ) );
					return FALSE;
				}
				MakeGemItem.nMinLevel = s.GetNumber();
				MakeGemItem.nMaxLevel = s.GetNumber();

				m_vecMakeGemItem.push_back( MakeGemItem );
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

// 아이템 착용 레벨에 따른 보석 종류
DWORD CUltimateWeapon::GetGemKind( DWORD dwLimitLevel )
{
	for( DWORD i=0; i<m_vecMakeGemItem.size(); i++ )
	{
		if( (int)( dwLimitLevel ) <= m_vecMakeGemItem[i].nMaxLevel &&
			(int)( dwLimitLevel ) >= m_vecMakeGemItem[i].nMinLevel )
			return m_vecMakeGemItem[i].dwItemID;
	}

	return NULL_ID;
}

#ifdef __WORLDSERVER
BOOL CUltimateWeapon::Load_UltimateWeapon()
{
	CScanner s;
	
	const TCHAR szFileName[]	= _T( "Ultimate_UltimateWeapon.txt" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	if( !s.Load( szFullPath ) )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return FALSE;
	}
	

	s.GetToken();
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "SET_GEM" ) )
			m_nSetGemProb = s.GetNumber();
		else if( s.Token == _T( "REMOVE_GEM" ) )
			m_nRemoveGemProb = s.GetNumber();
		else if( s.Token == _T( "GENERAL2UNIQUE" ) )
			m_nGen2UniProb = s.GetNumber();
		else if( s.Token == _T( "UNIQUE2ULTIMATE" ) )
			m_nUni2UltiProb = s.GetNumber();
		else if( s.Token == _T( "MAKE_GEM" ) )
		{
			__MAKEGEMPROB infoMakeGem;
			s.GetToken();	// {
			s.GetToken();
			while( *s.token != '}' )
			{
				int nEnchant = atoi( s.Token );
				infoMakeGem.dwGeneralProb = s.GetNumber();
				infoMakeGem.nGeneralNum = s.GetNumber();
				infoMakeGem.dwUniqueProb = s.GetNumber();
				infoMakeGem.nUniqueNum = s.GetNumber();

				m_mapMakeGemProb.insert( std::map<int, __MAKEGEMPROB>::value_type( nEnchant, infoMakeGem ) );
				s.GetToken();
			}
		}
		else if( s.Token == _T( "ULTIMATE_ENCHANT" ) )
		{
			int nNum = 0;
			int nProb= 0;
			s.GetToken(); // {
			s.GetToken();
			while( *s.token != '}' )
			{
				nNum = atoi( s.Token );
				nProb = s.GetNumber();
				m_mapUltimateProb.insert( std::map<int, int>::value_type( nNum, nProb ) );
				s.GetToken();
			}
		}
		s.GetToken();
	}	
	return TRUE;
}

// 보석 합성시 부여 되는 능력(능력)
DWORD CUltimateWeapon::GetGemAbilityKindRandom( DWORD dwGemItemid )
{
	if( m_vecGemAbilityKind.empty() == TRUE )
		return NULL_ID;

	int nRandom = xRandom( m_vecGemAbilityKind.size() );

	for(DWORD i=0; i<m_vecGemAbilityKind[nRandom].vecAbilityKind.size(); i++ )
	{
		if( m_vecGemAbilityKind[nRandom].vecAbilityKind[i].dwGemItemId == dwGemItemid )
			return m_vecGemAbilityKind[nRandom].vecAbilityKind[i].dwAbilityItemId;
	}
	return NULL_ID;
}

// 보석 생성 - 무기 아이템 파괴
int CUltimateWeapon::MakeGem( FLWSUser* pUser, OBJID objItemId, int & nNum )
{
	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	if( !IsUsableItem( pItemElem ) )
		return ULTIMATE_CANCEL;
	// 무기가 아닐 때
	if( pItemElem->GetProp()->dwItemKind1 != IK1_WEAPON )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "무기가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}
	// 일반무기나 유니크 아이템이 아닐 때
	if(	pItemElem->GetProp()->dwReferStat1 != WEAPON_GENERAL &&
		pItemElem->GetProp()->dwReferStat1 != WEAPON_UNIQUE )
	{
#ifdef __INTERNALSERVER
	pUser->AddText( "dwReferStat1 값이 WEAPON_GENERAL, WEAPON_UNIQUE 가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}
	
	// 장착되어 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
	// 레벨에 해당하는 보석 
	DWORD dwItemId = GetGemKind( static_cast<DWORD>( pItemElem->GetProp()->nMinLimitLevel ) );
	if( dwItemId == NULL_ID )
		return ULTIMATE_CANCEL;

	int nOpt = pItemElem->GetAbilityOption();
	std::map<int, __MAKEGEMPROB>::iterator it = m_mapMakeGemProb.find( nOpt );
	
	if( it == m_mapMakeGemProb.end() )
		return ULTIMATE_CANCEL;

	DWORD dwProb = it->second.dwGeneralProb;
	int nItemNum = it->second.nGeneralNum;
	if( pItemElem->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		dwProb = it->second.dwUniqueProb;
		nItemNum = it->second.nUniqueNum;
	}
	// 인벤토리가 부족할 때 - 취소
	FLItemElem itemElemTemp;
	itemElemTemp.m_dwItemId = dwItemId;
	itemElemTemp.m_nItemNum = nItemNum;
	PT_ITEM_SPEC pItemprop = itemElemTemp.GetProp();
	if( !pItemprop )
	{
		return ULTIMATE_CANCEL;
	}

	if( pUser->m_Inventory.IsFull( &itemElemTemp, itemElemTemp.m_nItemNum ) == TRUE )
	{
		return ULTIMATE_INVENTORY;
	}
	
	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_MAKEGEM";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_MAKEGEM" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );	
	pUser->RemoveItem( objItemId, 1 );
	
	nNum = 0;
	int nRandom = xRandom( 1000000 );
	if( (DWORD)( nRandom ) < dwProb )
	{
		nNum = nItemNum;
		FLItemElem itemElem;
		itemElem.m_dwItemId = dwItemId;
		itemElem.m_nItemNum	= nItemNum;
		itemElem.SetSerialNumber();
		itemElem.m_nHitPoint	= 0;

		pUser->CreateItem( &itemElem );
		//aLogItem.RecvName = "ULTIMATE_MAKEGEM_SUCCESS";
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_MAKEGEM_SUCCESS" );
		g_DPSrvr.OnLogItem( aLogItem, &itemElem, nItemNum );
		return ULTIMATE_SUCCESS;
	}
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_MAKEGEM_FAILED" );
	g_DPSrvr.OnLogItem( aLogItem );
	return ULTIMATE_FAILED;
}

// 무기에 보석 합성
int CUltimateWeapon::SetGem( FLWSUser* pUser, OBJID objItemId, OBJID objGemItemId )
{
	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	FLItemElem* pGemItemElem	= pUser->m_Inventory.GetAtId( objGemItemId );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pGemItemElem ) )
		return ULTIMATE_CANCEL;

	// 얼터멋 웨폰이 아닐 때
	if( pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE 
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		&& !pItemElem->GetProp()->IsBarunaWeapon() 
#endif
		)
		return ULTIMATE_ISNOTULTIMATE;


	// 장착되어 있으면 중단
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
		
//sun: 12, 무기 피어싱
	size_t nCount=0;
	for( ; nCount < pItemElem->GetUltimatePiercingSize(); nCount++ )
		if( pItemElem->GetUltimatePiercingItemID( nCount ) == 0 )
			break;
		
	// 빈곳이 없으면 중단
//sun: 12, 무기 피어싱
	if( nCount == pItemElem->GetUltimatePiercingSize() )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_GEMSPACE, "" );
		return ULTIMATE_CANCEL;
	}

	DWORD dwSetItemId = GetGemAbilityKindRandom( pGemItemElem->m_dwItemId );
	if( dwSetItemId == NULL_ID )
		return ULTIMATE_CANCEL;
	
	// 보석 삭제
	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_PIERCING";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_PIERCING" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pGemItemElem, 1 );	
	pUser->RemoveItem( objGemItemId, 1 );
	
	int nRandom = xRandom( 1000000 );
	if( nRandom < m_nSetGemProb )
	{
		// 보석 합성
//sun: 12, 무기 피어싱
		pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_ULTIMATE_PIERCING, MAKELONG( nCount, dwSetItemId ) );

		//aLogItem.RecvName = "ULTIMATE_PIERCING_SUCCESS";
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_PIERCING_SUCCESS" );
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
		return ULTIMATE_SUCCESS;	
	}
	//aLogItem.RecvName = "ULTIMATE_PIERCING_FAILED";
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_PIERCING_FAILED" );
	g_DPSrvr.OnLogItem( aLogItem, pItemElem );
	return ULTIMATE_FAILED;
}

// 무기에 마지막으로 합성된 보석 제거
int CUltimateWeapon::RemoveGem( FLWSUser* pUser, OBJID objItemId, OBJID objItemGem )
{
	FLItemElem* pItemElem	= pUser->m_Inventory.GetAtId( objItemId );
	FLItemElem* pItemElemGem	= pUser->m_Inventory.GetAtId( objItemGem );
	if( !IsUsableItem( pItemElem ) || !IsUsableItem( pItemElemGem ) )
		return ULTIMATE_CANCEL;
	
	if( pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE 
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		&& !pItemElem->GetProp()->IsBarunaWeapon()
#endif		
		)
		return ULTIMATE_ISNOTULTIMATE;
	
	// 합성된 보석이 없을 때 X
//sun: 12, 무기 피어싱
	if( pItemElem->GetUltimatePiercingItemID( 0 ) == 0 )
		return ULTIMATE_CANCEL;

	if( pItemElemGem->m_dwItemId != ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE )
		&& pItemElemGem->m_dwItemId != ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ) 
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		&& pItemElemGem->m_dwItemId != ITEM_INDEX( 25584, II_GEN_MAT_BARUNA_CANCEL ) 
#endif
		)
		return ULTIMATE_CANCEL;

	if ( pItemElem->GetProp()->IsBarunaWeapon() && pItemElemGem->m_dwItemId != ITEM_INDEX( 25584, II_GEN_MAT_BARUNA_CANCEL ) )
		return ULTIMATE_CANCEL;
 
	// 장착 되어 있을 때 X
	if( pUser->m_Inventory.IsEquip( objItemId ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}	

	// 문스톤 삭제
	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_PIERCING_REMOVE" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem, 1 );
	pUser->RemoveItem( objItemGem, 1 );
	
	int nRandom = xRandom( 1000000 );
	if( nRandom < m_nRemoveGemProb )
	{
		// 성공시 - 보석 제거
//sun: 12, 무기 피어싱
		for( int i=pItemElem->GetUltimatePiercingSize()-1; i>=0; i-- )
		{
			if( pItemElem->GetUltimatePiercingItemID( i ) != 0 )
			{
				pUser->UpdateItem( (BYTE)( pItemElem->m_dwObjId ), UI_ULTIMATE_PIERCING, MAKELONG( i, 0 ) );
				//aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE_SUC";
				FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_PIERCING_REMOVE_SUC" );
				g_DPSrvr.OnLogItem( aLogItem, pItemElem, 1 );
				break;
			}
		}
		return ULTIMATE_SUCCESS;
	}
	//aLogItem.RecvName = "ULTIMATE_PIERCING_REMOVE_FAIL";
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_PIERCING_REMOVE_FAIL" );
	g_DPSrvr.OnLogItem( aLogItem, pItemElem );
	return ULTIMATE_FAILED;
}

// 빛나는 오리칼쿰 생성 - 오리칼쿰 5개, 문스톤 5개 합성

//############# neodeath : 레알 문딩이 코드니 유지보수하지말고 새로 만들 것 
int CUltimateWeapon::MakeItem( FLWSUser* pUser, OBJID* objItemId )
{
	std::map<OBJID, int> mapObjId1;
	std::map<OBJID, int> mapObjId2;

	for( int i=0; i<5; i++ )
	{
		std::map<OBJID, int>::iterator it1 = mapObjId1.find(objItemId[i]);
		if( it1 != mapObjId1.end() )
			++(it1)->second;
		else
			mapObjId1.insert(std::map<OBJID, int>::value_type( objItemId[i], 1 ));
		
		std::map<OBJID, int>::iterator it2 = mapObjId2.find(objItemId[i+5]);
		if( it2 != mapObjId2.end() )
			++(it2)->second;
		else
			mapObjId2.insert(std::map<OBJID, int>::value_type( objItemId[i+5], 1 ));
	}

	int nItemCount = 0;
	for( std::map<OBJID, int>::iterator it = mapObjId1.begin() ; it != mapObjId1.end() ; ++it )
	{
		FLItemElem* pItemElem;
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		if( !IsUsableItem( pItemElem ) )
			return ULTIMATE_CANCEL;
		
		if( pItemElem->GetProp()->dwID != ITEM_INDEX( 2035, II_GEN_MAT_ORICHALCUM01 )
			&& pItemElem->GetProp()->dwID != ITEM_INDEX( 2082, II_GEN_MAT_ORICHALCUM01_1 ) )
			return ULTIMATE_CANCEL;
		if( it->second > pItemElem->m_nItemNum )
			return ULTIMATE_CANCEL;

		nItemCount += it->second;
	}
	if( nItemCount != 5 )
		return ULTIMATE_CANCEL;

	nItemCount = 0;
	for( std::map<OBJID, int>::iterator it = mapObjId2.begin() ; it != mapObjId2.end() ; ++it )
	{
		FLItemElem* pItemElem;
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		if( !IsUsableItem( pItemElem ) )
			return ULTIMATE_CANCEL;
		
		if( pItemElem->GetProp()->dwID != ITEM_INDEX( 2036, II_GEN_MAT_MOONSTONE )
			&& pItemElem->GetProp()->dwID != ITEM_INDEX( 2083, II_GEN_MAT_MOONSTONE_1 ) )
			return ULTIMATE_CANCEL;
		if( it->second > pItemElem->m_nItemNum )
			return ULTIMATE_CANCEL;
		
		nItemCount += it->second;
	}

	if( nItemCount != 5 )
		return ULTIMATE_CANCEL;
	
	//인벤토리가 부족할 때 - 취소
	FLItemElem itemElemTemp;
	itemElemTemp.m_dwItemId = ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 );
	itemElemTemp.m_nItemNum = 1;
	if( !itemElemTemp.GetProp() || pUser->m_Inventory.IsFull( &itemElemTemp, itemElemTemp.m_nItemNum ) )
	{
		return ULTIMATE_INVENTORY;
	}
	
	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_MAKEITEM";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_MAKEITEM" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	
	FLItemElem* pItemElem;
	for( std::map<OBJID, int>::iterator it = mapObjId1.begin() ; it != mapObjId1.end() ; ++it )
	{
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, it->second );	
		pUser->RemoveItem( it->first, it->second );
	}
	for( std::map<OBJID, int>::iterator it = mapObjId2.begin() ; it != mapObjId2.end() ; ++it )
	{
		pItemElem = pUser->m_Inventory.GetAtId( it->first );
		g_DPSrvr.OnLogItem( aLogItem, pItemElem, it->second );	
		pUser->RemoveItem( it->first, it->second );
	}

	FLItemElem itemElem;
	itemElem.m_dwItemId = ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 );
	itemElem.m_nItemNum	= 1;
	itemElem.SetSerialNumber();
	itemElem.m_nHitPoint	= 0;
	
	pUser->CreateItem( &itemElem );
	//aLogItem.RecvName = "ULTIMATE_MAKEITEM_SUCCESS";
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_MAKEITEM_SUCCESS" );
	g_DPSrvr.OnLogItem( aLogItem, &itemElem, 1 );
	
	return ULTIMATE_SUCCESS;
}

// 무기변환(일반->유니크, 유니크->얼터멋)
int CUltimateWeapon::TransWeapon( FLWSUser* pUser, OBJID objItem, OBJID objGem1, OBJID objGem2 )
{
	FLItemElem* pItemElemWeapon	= pUser->m_Inventory.GetAtId( objItem );
	FLItemElem* pItemElemGem1	= pUser->m_Inventory.GetAtId( objGem1 );
	FLItemElem* pItemElemGem2	= pUser->m_Inventory.GetAtId( objGem2 );
	if( IsUsableItem( pItemElemWeapon ) == FALSE
		|| IsUsableItem( pItemElemGem1 ) == FALSE
		|| IsUsableItem( pItemElemGem2 ) == FALSE )
		return ULTIMATE_CANCEL;

	PT_ITEM_SPEC pMainProp = g_xSpecManager->GetSpecItem( pItemElemWeapon->m_dwItemId );
	if( pMainProp == NULL )
	{
		return ULTIMATE_CANCEL;
	}
	
	// 장착하고 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItem ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}
		
	// 무기가 아니면 취소
	if( pItemElemWeapon->GetProp()->dwItemKind1 != IK1_WEAPON )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "무기가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}

	// 타겟 ID 가 없을 때
	if( pItemElemWeapon->GetProp()->dwReferTarget1 == NULL_ID )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "dwReferTarget1 에 변환될 아이템 ID가 없음." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;	
	}
	
	// 일반, 유니크 아이템이 아닐 때
	if( pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_GENERAL &&
		  pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_UNIQUE )
	{
#ifdef __INTERNALSERVER
		pUser->AddText( "dwReferStat1 값이 WEAPON_GENERAL, WEAPON_UNIQUE 가 아님." );
#endif // __INTERNALSERVER
		return ULTIMATE_CANCEL;
	}

	DWORD dwItemId = GetGemKind( static_cast<DWORD>( pItemElemWeapon->GetProp()->nMinLimitLevel ) );
	if( dwItemId != pItemElemGem1->m_dwItemId || pItemElemGem2->m_dwItemId != ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ) )
		return ULTIMATE_CANCEL;

	CString strTemp = "UNIQUE_"; 
	CString strLog;
	int nProb = m_nGen2UniProb;
	if( pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		if( pItemElemWeapon->GetAbilityOption() != 10 )
			return ULTIMATE_CANCEL;
		nProb = m_nUni2UltiProb;
		strTemp = "ULTIMATE_";
	}
	// propItem.txt -> dwReferTarget2(참조타겟2)에 값이 변환 확률이 된다.
	// 값이 없으면 Ultimate_UltimateWeapon.txt에 정의된 값이 변환 확률이 된다.
	if( pItemElemWeapon->GetProp()->dwReferTarget2 != NULL_ID )
		nProb = pItemElemWeapon->GetProp()->dwReferTarget2;
	
	BOOL bSmelprot3	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 ) )
		&& pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
	{
		bSmelprot3	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 ) );
		
	}

	LogItemInfo aLogItem;
//	aLogItem.Action = "-";
//	aLogItem.SendName = pUser->GetName();
//	strLog = strTemp + "TRANS";
//	aLogItem.RecvName = (LPCTSTR)strLog;
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
    FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	strLog = strTemp + "TRANS";
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), (LPCTSTR)strLog );

	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem1, 1 );
	pUser->RemoveItem( objGem1, 1 );
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem2, 1 );
	pUser->RemoveItem( objGem2, 1 );

	int nAddProp = g_xApplyItemEffect->GetAdjValueGeneralEnchantRateBuff( pUser, pItemElemWeapon, IK3_ULTIMATE_UPGRADE_RATE, true );
	if( nAddProp > 0 )
	{
		nAddProp *= 100;
		nProb += nAddProp;
	}

	int nRandom = xRandom( 1000000 );
	if( nRandom < nProb )
	{
		FLItemElem CreateItem;
		CreateItem	= *pItemElemWeapon;
		CreateItem.m_dwItemId = pItemElemWeapon->GetProp()->dwReferTarget1;
		CreateItem.SetSerialNumber();

		if( pItemElemWeapon->GetProp()->dwReferStat1 == WEAPON_UNIQUE )
			CreateItem.SetAbilityOption( 0 );
		

		g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
		pUser->RemoveItem( objItem, 1 );
		if( pUser->CreateItem( &CreateItem ) )
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_SUCCESS";
			strLog = strTemp + "TRANS_SUCCESS";
			//aLogItem.RecvName = (LPCTSTR)strLog;
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), (LPCTSTR)strLog );
			g_DPSrvr.OnLogItem( aLogItem, &CreateItem, 1 );
			return ULTIMATE_SUCCESS;
		}
	}
	else //실패
	{
		if( !bSmelprot3 )
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_FAILED";
			strLog = strTemp + "TRANS_FAILED";
			//aLogItem.RecvName = (LPCTSTR)strLog;
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), (LPCTSTR)strLog );
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
			pUser->RemoveItem( objItem, 1 );
		}
		else
		{
			//aLogItem.RecvName = "ULTIMATE_TRANS_PROTECT";
			strLog = strTemp + "TRANS_PROTECT";
			//aLogItem.RecvName = (LPCTSTR)strLog;
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), (LPCTSTR)strLog );
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon );
		}
	}
	
	return ULTIMATE_FAILED;
}

// 얼터멋 웨폰 제련
int CUltimateWeapon::EnchantWeapon( FLWSUser* pUser, OBJID objItem, OBJID objItemGem )
{
	FLItemElem* pItemElemWeapon	= pUser->m_Inventory.GetAtId( objItem );
	FLItemElem* pItemElemGem	= pUser->m_Inventory.GetAtId( objItemGem );
	if( IsUsableItem( pItemElemWeapon ) == FALSE || IsUsableItem( pItemElemGem ) == FALSE )
		return ULTIMATE_CANCEL;

	//얼터멋 웨폰이 아니거나 빛나는 오리칼쿰이 아니면 취소
	if( pItemElemWeapon->GetProp()->dwReferStat1 != WEAPON_ULTIMATE ||
		pItemElemGem->m_dwItemId != ITEM_INDEX( 2034, II_GEN_MAT_ORICHALCUM02 ) )
		return ULTIMATE_ISNOTULTIMATE;

	// 장착하고 있으면 취소
	if( pUser->m_Inventory.IsEquip( objItem ) )
	{
		pUser->AddDefinedText( TID_GAME_ULTIMATE_ISEQUIP , "" );
		return ULTIMATE_CANCEL;
	}

	int pAbilityOpt = pItemElemWeapon->GetAbilityOption();
	if( (pAbilityOpt+1) > 10 )
	{
		pUser->AddDefinedText( TID_UPGRADE_MAXOVER , "" );
		return ULTIMATE_CANCEL;
	}

	BOOL bSmelprot3	= FALSE;
	if( pUser->HasBuff( BUFF_ITEM, ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 ) ) )
	{
		bSmelprot3	= TRUE;
		pUser->RemoveBuff( BUFF_ITEM, ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 ) );
		
	}
	
	//보석 소진
	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_ENCHANT";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_ENCHANT" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = aLogItem.Gold2 = pUser->GetGold();
	g_DPSrvr.OnLogItem( aLogItem, pItemElemGem, 1 );	
	pUser->RemoveItem( objItemGem, 1 );

	int nRandom = xRandom( 1000000 );
	std::map<int, int>::iterator it = m_mapUltimateProb.find( pAbilityOpt+1 );
	if( it == m_mapUltimateProb.end() )
		return ULTIMATE_CANCEL;
	
	if( nRandom < it->second )
	{
		pUser->UpdateItem( (BYTE)pItemElemWeapon->m_dwObjId, UI_AO, ++pAbilityOpt );
		if( pAbilityOpt > 5 )
//sun: 12, 무기 피어싱
			pUser->UpdateItem( (BYTE)pItemElemWeapon->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, pAbilityOpt - 5 );

		//aLogItem.RecvName = "ULTIMATE_ENCHANT_SUCCESS";
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_ENCHANT_SUCCESS" );
		g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
		return ULTIMATE_SUCCESS;
	}
	else
	{
		if( bSmelprot3 )	// 보호의 두루마리(최상급) 사용시
		{
			//aLogItem.RecvName = "ULTIMATE_ENCHANT_PROTECT";
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_ENCHANT_PROTECT" );
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon );
		}
		else	// 얼터멋 웨폰 파괴
		{
			//aLogItem.RecvName = "ULTIMATE_ENCHANT_FAILED";
			FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_ENCHANT_FAILED" );
			g_DPSrvr.OnLogItem( aLogItem, pItemElemWeapon, 1 );
			pUser->RemoveItem( objItem, 1 );
		}
	}	
	return ULTIMATE_FAILED;
}


BYTE CUltimateWeapon::SmeltSafetyUltimate( FLWSUser* pUser, FLItemElem* pItemMain, FLItemElem* pItemMaterial, FLItemElem* pItemProtScr )
{
	if( IsUsableItem( pItemMain ) == FALSE || IsUsableItem( pItemMaterial ) == FALSE || IsUsableItem( pItemProtScr ) == FALSE )
	{
		return 0;
	}

	PT_ITEM_SPEC pMainProp		= g_xSpecManager->GetSpecItem( pItemMain->m_dwItemId );
	PT_ITEM_SPEC pMaterialProp	= g_xSpecManager->GetSpecItem( pItemMaterial->m_dwItemId );
	PT_ITEM_SPEC pProtScrProp	= g_xSpecManager->GetSpecItem( pItemProtScr->m_dwItemId );
	if( pMainProp == NULL || pMaterialProp == NULL || pProtScrProp == NULL )
	{
		return 0;
	}

	if( pMainProp->IsUpgradeAble( IUTYPE_ENCHANT_GENERAL ) == FALSE )
	{
		return 0;
	}

	//	얼터멋웨폰이 아니면 리턴
	if( pMainProp->dwReferStat1 != WEAPON_ULTIMATE )
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}

	//	최상급 보호의 두루마리가 아니면 리턴
	if( pProtScrProp->dwID != ITEM_INDEX( 10488, II_SYS_SYS_SCR_SMELPROT3 ))
	{
		//pUser->AddDefinedText( TID_GAME_NOTEQUALITEM );
		return 0;
	}

	//	제련수치가 max치를 넘었을때 리턴
	if( pItemMain->GetAbilityOption() >= 10 )
	{
		//pUser->AddDefinedText( TID_UPGRADE_MAXOVER );
		return 3;
	}

#ifdef __ENCHANT_BARUNA16

	if( pMainProp->IsBaruna() == TRUE )
	{
		return 0;
	}

#endif // __ENCHANT_BARUNA16


	LogItemInfo aLogItem;
	//aLogItem.Action = "-";
	//aLogItem.SendName = pUser->GetName();
	//aLogItem.RecvName = "ULTIMATE_ENC_SMELTSAFETY";
	FLStrcpy( aLogItem.Action, _countof( aLogItem.Action ), "-" );
	FLStrcpy( aLogItem.SendName, _countof( aLogItem.SendName ), pUser->GetName() );
	FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_ENC_SMELTSAFETY" );
	aLogItem.WorldId = pUser->GetWorld()->GetID();
	aLogItem.Gold = pUser->GetGold();
	aLogItem.Gold2 = pUser->GetGold();
	
	// 제련아템 삭제 - 성공이던, 실패던...
	g_DPSrvr.OnLogItem( aLogItem, pItemMaterial, pItemMaterial->m_nItemNum );
	pUser->RemoveItem( pItemMaterial->m_dwObjId, 1 );
	//	두루마리 로그남길것
	g_dpDBClient.SendLogSMItemUse( "1", pUser, pItemProtScr, pProtScrProp );
	g_dpDBClient.SendLogSMItemUse( "2", pUser, NULL, pProtScrProp );

	pUser->RemoveItem( pItemProtScr->m_dwObjId, 1 );

	// 제련 확률
	std::map<int, int>::iterator it = m_mapUltimateProb.find( pItemMain->GetAbilityOption() + 1 );
	if( it == m_mapUltimateProb.end() )
		return 0;
	
	if( (int)( xRandom( 1000000 ) ) > it->second )
	{
		// 실패
		pUser->AddPlaySound( SND_INF_UPGRADEFAIL );
		if( ( pUser->IsMode( TRANSPARENT_MODE ) ) == 0 )
			g_xWSUserManager->AddCreateSfxObj( ( CMover * )pUser, XI_INDEX( 1715, XI_INT_FAIL ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );

		//aLogItem.RecvName = "ULTIMATE_ENC_FAIL_SMELTSAFETY";
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_ENC_FAIL_SMELTSAFETY" );
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 2;
	}
	else
	{
		// 성공
		pUser->AddPlaySound( SND_INF_UPGRADESUCCESS );		
		if((pUser->IsMode( TRANSPARENT_MODE ) ) == 0 )
			g_xWSUserManager->AddCreateSfxObj( ( CMover * )pUser, XI_INDEX( 1714, XI_INT_SUCCESS ), pUser->GetPos().x, pUser->GetPos().y, pUser->GetPos().z );
		
		pUser->UpdateItem( (BYTE)pItemMain->m_dwObjId, UI_AO, pItemMain->GetAbilityOption() + 1 );
		if( pItemMain->GetAbilityOption() > 5 )
			pUser->UpdateItem( (BYTE)pItemMain->m_dwObjId, UI_ULTIMATE_PIERCING_SIZE, pItemMain->GetAbilityOption() - 5 );
		
		//aLogItem.RecvName = "ULTIMATE_ENC_SUC_SMELTSAFETY";
		FLStrcpy( aLogItem.RecvName, _countof( aLogItem.RecvName ), "ULTIMATE_ENC_SUC_SMELTSAFETY" );
		g_DPSrvr.OnLogItem( aLogItem, pItemMain, pItemMain->m_nItemNum );

		return 1;
	}
//	return 0;
}
#endif // __WORLDSERVER

// 보석 합성 후 능력치 적용
void CUltimateWeapon::SetDestParamUltimate( CMover* pMover, FLItemElem* pItemElem, BOOL bEquip, DWORD dwParts )
{
	
	if( dwParts != PARTS_RWEAPON )
		return;
	
	std::map<int, int> mapDst = GetDestParamUltimate( pItemElem );
	for(std::map<int, int>::iterator it = mapDst.begin(); it != mapDst.end(); it++ )
	{
		if( bEquip )
			pMover->SetDestParam( it->first, it->second, NULL_CHGPARAM );
		else 
			pMover->ResetDestParam( it->first, it->second, TRUE );
	}
}

// 합성된 보석들의 종류 및 능력
std::map<int, int> CUltimateWeapon::GetDestParamUltimate( FLItemElem* pItemElem )
{
	std::map<int, int> mapDst;  
	if( !pItemElem || (pItemElem->GetProp()->dwReferStat1 != WEAPON_ULTIMATE 
#ifdef BARUNA_UPGRADE_ENHANCEMENT_GEM
		&& !pItemElem->GetProp()->IsBarunaWeapon()
#endif
	) )
		return mapDst;
	
	std::map<DWORD, int> mapItem;		
//sun: 12, 무기 피어싱
	for( size_t i=0; i<pItemElem->GetUltimatePiercingSize(); i++ )
	{
		if( pItemElem->GetUltimatePiercingItemID( i ) == 0 )
			break;

		std::map<DWORD, int>::iterator it=mapItem.find( pItemElem->GetUltimatePiercingItemID( i ) );
		if( it==mapItem.end() )
			mapItem.insert( std::map<DWORD, int>::value_type( pItemElem->GetUltimatePiercingItemID( i ), 1 ) );
		else
			it->second++;
	}
	
	for( std::map<DWORD, int>::iterator it=mapItem.begin(); it!=mapItem.end(); it++ )
	{
		PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( it->first );
		int nResultDst = GetDST( it->first, it->second, pItemProp->dwReferStat2 );
		
		std::map<int, int>::iterator iter=mapDst.find( pItemProp->dwReferStat2 );
		if( iter==mapDst.end() )
			mapDst.insert( std::map<int, int>::value_type( pItemProp->dwReferStat2, nResultDst ) );
		else
			iter->second += nResultDst;
	}
	return mapDst;
}

// 합성된 보석의 종류와 갯수에 따른 능력치
int CUltimateWeapon::GetDST( DWORD dwItemId, int nItemNum, int nDSTInfo )
{
	int nResultDST = 0;
	__GEMABILITYKIND	GemAbilityKind;

	BOOL bFind = FALSE;
	for( DWORD i=0; i<m_vecGemAbilityKind.size(); i++ )
	{
		if( m_vecGemAbilityKind[i].nAbility == nDSTInfo )
		{
			GemAbilityKind = m_vecGemAbilityKind[i];
			bFind = TRUE;
			break;
		}
	}

	if( !bFind )
		return 0;

	for( DWORD i = 0 ; i < GemAbilityKind.vecAbilityKind.size() ; ++i )
	{
		if( dwItemId == GemAbilityKind.vecAbilityKind[i].dwAbilityItemId )
		{
			nResultDST = GemAbilityKind.vecAbilityKind[i].vecAbility[nItemNum-1];
			break;
		}
	}	
	return nResultDST;
}
