
#include "StdAfx.h"
#include "FLMadrigalGift.h"
#include "../_CommonDefine/Packet/FLPacketMadrigalGift.h"


FLMadrigalGift::FLMadrigalGift()
: m_bActivate( false ), m_nLogOutPoint( 0 ), m_nKeepConnectionPoint( 0 ), m_nMaxGiftPoint( 0 )
{
}

FLMadrigalGift::~FLMadrigalGift()
{
}

FLMadrigalGift*	FLMadrigalGift::GetInstance()
{
	static	FLMadrigalGift sMadrigalGift;

	return & sMadrigalGift;
}

bool	FLMadrigalGift::IsActivate() const
{
	return m_bActivate;
}

void	FLMadrigalGift::SetActivate( const bool bActivate )
{
	m_bActivate = bActivate;
}

void	FLMadrigalGift::Init()
{
	for( nsMadrigalGift::RewardItem_PlayerMap::iterator pos = m_mapRewardItemData.begin(); pos != m_mapRewardItemData.end(); ++pos )
	{
		pos->second.clear();
	}
	m_mapRewardItemData.clear();
	
	m_mapGiftStepData.clear();
	m_mapGiftPointData.clear();

	m_nLogOutPoint				= 0;
	m_nKeepConnectionPoint		= 0;
	m_nMaxGiftPoint				= 0;
}

bool	FLMadrigalGift::LoadScript()
{
	const bool bRet	= (		LoadRewardItemScript( _T( "MadrigalGiftItem.csv" ) )
						&& LoadStepPointScript( _T( "MadrigalGiftSet.txt" ) )
						&& LoadAddPointScript( _T( "MadrigalGiftPoint.txt" ) )		);

	SetActivate( bRet );

	if( bRet == false )
	{
		Init();
	}

	return	bRet;
}

bool	FLMadrigalGift::LoadRewardItemScript( const TCHAR* pszFileName )
{
	CScript kScanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kScanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	nsMadrigalGift::T_GIFT_PLAYER_DATA	kPrevPlayerData;

	DWORD dwLine = 0;

	DWORD dwIndex = static_cast<DWORD>( kScanner.GetNumber( TRUE ) );

	while( kScanner.tok != FINISHED )
	{
		++dwLine;

		kScanner.GetToken( TRUE );
		std::string strPlayerGrade	= kScanner.Token;
		const DWORD dwGrade			= PlayerGradeStringToDefine( strPlayerGrade );
		const int	nMinLevel		= kScanner.GetNumber( TRUE );
		const int	nMaxLevel		= kScanner.GetNumber( TRUE );

		nsMadrigalGift::T_GIFT_PLAYER_DATA	kPlayerData( dwGrade, nMinLevel, nMaxLevel );

		if( dwIndex > 0 )
		{
			if( dwGrade == NULL_ID || ( nMinLevel > nMaxLevel ) )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ PLAYER DATA SETTING INVALID. Line(%d), Grade(%s), MinLevel(%d), MaxLevel(%d) ]" )
					, dwLine, strPlayerGrade.c_str(), nMinLevel, nMaxLevel);
				FLASSERT( 0 );
				return false;
			}

			nsMadrigalGift::T_GIFT_PLAYER_DATA	kPlayerData( dwGrade, nMinLevel, nMaxLevel );

			if( kPrevPlayerData == kPlayerData )
			{
				FLASSERT( 0 );
				return false;
			}
			else
			{
				kPrevPlayerData	= kPlayerData;
			}
		}

		nsMadrigalGift::T_GIFT_ITEM_DATA		kItemData;

		kItemData.dwItemID			= static_cast<DWORD>( kScanner.GetNumber( TRUE ) );
		kItemData.dwRewardProb		= static_cast<DWORD>( kScanner.GetNumber( TRUE ) );
		kItemData.dwMinQuantity		= static_cast<DWORD>( kScanner.GetNumber( TRUE ) );
		kItemData.dwMaxQuantity		= static_cast<DWORD>( kScanner.GetNumber( TRUE ) );

		const PT_ITEM_SPEC pItemProp	= g_xSpecManager->GetSpecItem( kItemData.dwItemID );
		if( pItemProp == NULL )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ ITEM ID SETTING INVALID. Line(%d), ItemID(%d) ]" )
				, dwLine, kItemData.dwItemID );
			FLASSERT( 0 );
			return false;
		}

		if( kItemData.dwMinQuantity > kItemData.dwMaxQuantity )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ ITEM COUNT SETTING INVALID. Line(%d), ItemID(%d), MinCount(%d), MaxCount(%d) ]" )
				, dwLine, kItemData.dwItemID, kItemData.dwMinQuantity, kItemData.dwMaxQuantity );
			FLASSERT( 0 );
			return false;
		}

		nsMadrigalGift::RewardItem_PlayerMap::iterator itr = m_mapRewardItemData.find( kPrevPlayerData );
		if( itr != m_mapRewardItemData.end() )
		{
			nsMadrigalGift::T_GIFT_ITEM_DATA kDummy		= itr->second.back();
			kItemData.dwRewardProb	+= kDummy.dwRewardProb;
			
			itr->second.push_back( kItemData );
		}
		else
		{
			nsMadrigalGift::RewardItemVec vecRewardItem;
			vecRewardItem.push_back( kItemData );

			const bool bRet = m_mapRewardItemData.insert( nsMadrigalGift::RewardItem_PlayerMap::value_type( kPlayerData, vecRewardItem ) ).second;
			if( bRet == false )
			{
				FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATED DATA. Line(%d), Grade(%d), MinLevel(%d), MaxLevel(%d) ]" )
					, dwLine, kPlayerData.dwGrade, kPlayerData.nMinLevel, kPlayerData.nMaxLevel );
				FLASSERT( 0 );
				return false;
			}
		}

		if( kItemData.dwRewardProb > nsMadrigalGift::MAX_REWARD_PROB )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ ITEM PROB SETTING INVALID. Line(%d), ItemID(%d), Prob(%d), MaxProb(%d) ]" )
				, dwLine, kItemData.dwItemID, kItemData.dwRewardProb, nsMadrigalGift::MAX_REWARD_PROB );
			FLASSERT( 0 );
			return false;
		}

		dwIndex = static_cast<DWORD>( kScanner.GetNumber( TRUE ) );
		if( dwIndex > 0 && kItemData.dwRewardProb != nsMadrigalGift::MAX_REWARD_PROB )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ ITEM PROB SUM IS NOT MAX VALUE. Line(%d), ItemID(%d), SumProb(%d), MaxProb(%d) ]" )
				, dwLine, kItemData.dwItemID, kItemData.dwRewardProb, nsMadrigalGift::MAX_REWARD_PROB );
			FLASSERT( 0 );
			return false;
		}
	}

	return ( dwLine != 0 ) ? true : false;
}

bool	FLMadrigalGift::LoadStepPointScript( const TCHAR* pszFileName )
{
	CScript kScanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kScanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	DWORD dwLine = 0;

	BYTE byStep = static_cast<BYTE>( kScanner.GetNumber() );

	while( kScanner.tok != FINISHED )
	{
		++dwLine;

		nsMadrigalGift::T_GIFT_POINT	kPoint;

		kPoint.nMinPoint	= kScanner.GetNumber();
		kPoint.nMaxPoint	= kScanner.GetNumber();
		if( kPoint.nMinPoint > kPoint.nMaxPoint )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ POINT SETTING INVALID. Line:(%d), Min:(%d), Max:(%d) ]" )
				, dwLine, kPoint.nMinPoint, kPoint.nMaxPoint );
			FLASSERT( 0 );
			return false;
		}

		nsMadrigalGift::T_GIFT_STEP		kStep;
		
		kStep.byStep		= byStep;
		kStep.nAddLevel		= kScanner.GetNumber();

		if( m_mapGiftStepData.insert( nsMadrigalGift::GiftStepDataMap::value_type( kPoint, kStep ) ).second == false )
		{
			FLASSERT( 0 );
			return false;
		}

		if( m_mapGiftPointData.insert( nsMadrigalGift::GiftPointDataMap::value_type( byStep, kPoint ) ).second == false )
		{
			FLASSERT( 0 );
			return false;
		}

		m_nMaxGiftPoint	= ( kPoint.nMaxPoint > m_nMaxGiftPoint ) ? kPoint.nMaxPoint : m_nMaxGiftPoint;

		byStep = static_cast<BYTE>( kScanner.GetNumber() );
	}

	return ( dwLine != 0 ) ? true : false;
}

bool	FLMadrigalGift::LoadAddPointScript( const TCHAR* pszFileName )
{
	CScript kScanner;

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	if( kScanner.Load( szFullPath, FALSE ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return false;
	}

	m_nLogOutPoint			= kScanner.GetNumber();

	m_nKeepConnectionPoint	= kScanner.GetNumber();

	if( m_nLogOutPoint <= 0 || m_nKeepConnectionPoint <= 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ POINT SETTING INVALID. LogOutPoint:(%d), KeepConnectionPoint:(%d) ]" )
			, m_nLogOutPoint, m_nKeepConnectionPoint );
		FLASSERT( 0 );
		return false;
	}

	return true;
}

DWORD	FLMadrigalGift::PlayerGradeStringToDefine( std::string & strPlayerGrade ) const
{
	std::transform( strPlayerGrade.begin(), strPlayerGrade.end(), strPlayerGrade.begin(), toupper );

	if( strPlayerGrade == "NORMAL" )
	{
		return LEGEND_CLASS_NORMAL;
	}
	else if( strPlayerGrade == "MASTER" )
	{
		return LEGEND_CLASS_MASTER;
	}
	else if( strPlayerGrade == "HERO")
	{
		return LEGEND_CLASS_HERO;
	}
	else if( strPlayerGrade == "LEGEND_HERO")
	{
		return LEGEND_CLASS_LEGEND_HERO;
	}
	else
	{
		return NULL_ID;
	}
}

bool	FLMadrigalGift::CalcLoginPoint( FLWSUser* pUser, const time_t tLogOut ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( IsActivate() == false )
	{
		return false;
	}

	pUser->m_kMadrigalGiftPoint.dwGetPointTick	= ::GetTickCount();

	if( pUser->m_kMadrigalGiftPoint.nPoint < m_nMaxGiftPoint )
	{
		if( tLogOut <= 0 )
		{
			return true;
		}

		if( tLogOut < nsMadrigalGift::POINT_REWARD_TIME_GAP_SEC )
		{
			return true;
		}

		const int nGiftPoint	= static_cast<int>( ( tLogOut / nsMadrigalGift::POINT_REWARD_TIME_GAP_SEC ) * m_nLogOutPoint );

		const float fRate		= GetGiftPointRate( pUser );

		pUser->m_kMadrigalGiftPoint.nPoint		+= static_cast<int>( nGiftPoint * fRate );
	}

	if( pUser->m_kMadrigalGiftPoint.nPoint > m_nMaxGiftPoint )
	{
		pUser->m_kMadrigalGiftPoint.nPoint = m_nMaxGiftPoint;
	}

	return true;
}

bool	FLMadrigalGift::CalcKeepConnectionPoint( FLWSUser* pUser, const DWORD dwTick ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( pUser->m_kMadrigalGiftPoint.dwGetPointTick <= 0 || pUser->m_kMadrigalGiftPoint.dwGetPointTick > dwTick )
	{
		pUser->m_kMadrigalGiftPoint.dwGetPointTick	= dwTick;
		return false;
	}

	if( dwTick < pUser->m_kMadrigalGiftPoint.dwGetPointTick + ( nsMadrigalGift::POINT_REWARD_TIME_GAP_SEC * 1000 ) )
	{
		return false;
	}

	const int nPrevPoint	= pUser->m_kMadrigalGiftPoint.nPoint;
	pUser->m_kMadrigalGiftPoint.dwGetPointTick	= dwTick;

	if( pUser->m_kMadrigalGiftPoint.nPoint < m_nMaxGiftPoint )
	{
		const float fRate		= GetGiftPointRate( pUser );

		pUser->m_kMadrigalGiftPoint.nPoint			+= static_cast<int>( m_nKeepConnectionPoint * fRate );
	}

	if( pUser->m_kMadrigalGiftPoint.nPoint > m_nMaxGiftPoint )
	{
		pUser->m_kMadrigalGiftPoint.nPoint = m_nMaxGiftPoint;
	}

	if( nPrevPoint != pUser->m_kMadrigalGiftPoint.nPoint )
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool	FLMadrigalGift::CalcMonsterKillPoint( FLWSUser* pUser, CMover* pMover ) const
{
	if( IsValidObj( pUser ) == FALSE || pMover == NULL )
	{
		return false;
	}

	if( pUser->m_kMadrigalGiftPoint.nPoint >= m_nMaxGiftPoint )
	{
		return false;
	}

	if( pMover->GetType() != OT_MOVER || pMover->IsPlayer() == TRUE )
	{
		return false;
	}

	MoverProp* pMoverProp	= pMover->GetProp();
	if( pMoverProp == NULL )
	{
		return false;
	}

	const int nLevelGap		= pUser->GetLevel() - static_cast<int>( pMoverProp->dwLevel );
	if( nLevelGap > nsMadrigalGift::MAX_LEVEL_GAP )
	{
		return false;
	}

	const int nPrevPoint	= pUser->m_kMadrigalGiftPoint.nPoint;

	const float fRate		= GetGiftPointRate( pUser );

	pUser->m_kMadrigalGiftPoint.nPoint			+= static_cast<int>( pMoverProp->nMadrigalGiftPoint * fRate );;

	if( pUser->m_kMadrigalGiftPoint.nPoint > m_nMaxGiftPoint )
	{
		pUser->m_kMadrigalGiftPoint.nPoint = m_nMaxGiftPoint;
	}

	if( nPrevPoint != pUser->m_kMadrigalGiftPoint.nPoint )
	{
		return true;
	}
	else
	{
		return false;
	}
}

void	FLMadrigalGift::SendSystemActiveNoti( FLWSUser* pUser ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	FLSnapshotMadrigalGiftSystemActivationNoti toClient;
	toClient.bActivate	= IsActivate();

	pUser->AddPacket( &toClient );
}

void	FLMadrigalGift::SendGiftPointNoti( FLWSUser* pUser ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	nsMadrigalGift::T_GIFT_STEP kCurrGiftStep;

	if( GetGiftStep( pUser->m_kMadrigalGiftPoint.nPoint, kCurrGiftStep ) == false )
	{
		return;
	}

	nsMadrigalGift::T_GIFT_POINT kCurrGiftPoint;

	if( GetGiftPoint( kCurrGiftStep.byStep, kCurrGiftPoint ) == false )
	{
		return;
	}

	nsMadrigalGift::T_GIFT_POINT kNextGiftPoint;

	if( GetGiftPoint( ( kCurrGiftStep.byStep + 1 ), kNextGiftPoint ) == false )
	{
		kNextGiftPoint.nMinPoint	= kCurrGiftPoint.nMaxPoint;
	}

	nsMadrigalGift::T_GIFT_STEP kNextGiftStep;

	GetGiftStep( kNextGiftPoint.nMinPoint, kNextGiftStep );

	const int nBaseRewardLevel	= pUser->GetLevel() + GetAddRewardLevel( pUser );

	FLSnapshotMadrigalGiftPointNoti toClient;
	toClient.byCurrStep			= kCurrGiftStep.byStep;
	toClient.nCurrPoint			= ( kCurrGiftStep.byStep < kNextGiftStep.byStep )
		? ( pUser->m_kMadrigalGiftPoint.nPoint - kCurrGiftPoint.nMinPoint ) : pUser->m_kMadrigalGiftPoint.nPoint;
	toClient.nMaxPoint			= ( kCurrGiftStep.byStep < kNextGiftStep.byStep )
		? ( kNextGiftPoint.nMinPoint - kCurrGiftPoint.nMinPoint ) : kCurrGiftPoint.nMinPoint;
	toClient.nCurrRewardLevel	= nBaseRewardLevel + kCurrGiftStep.nAddLevel;
	toClient.nNextRewardLevel	= nBaseRewardLevel + kNextGiftStep.nAddLevel;
	toClient.bActivate			= ( kCurrGiftStep.byStep > 0 ) ? true : false;

	pUser->AddPacket( &toClient );
}

void	FLMadrigalGift::SendReceiveGiftItemAck( FLWSUser* pUser ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	nsMadrigalGift::T_GIFT_STEP kCurrGiftStep;
	GetGiftStep( pUser->m_kMadrigalGiftPoint.nPoint, kCurrGiftStep );

	FLSnapshotMadrigalGiftItemReceiveAck toClient;
	toClient.bActivate			= ( kCurrGiftStep.byStep > 0 ) ? true : false;

	pUser->AddPacket( &toClient );
}

float	FLMadrigalGift::GetGiftPointRate( FLWSUser* pUser ) const
{
	float fRate = 1.0f;

	if( IsValidObj( pUser ) == FALSE )
	{
		return fRate;
	}

	IBuff* pBuff	= pUser->GetBuffByIk3( IK3_MADRIGALBLESS );
	if( pBuff == NULL )
	{
		return fRate;
	}

	const PT_ITEM_SPEC pItemProp	= pBuff->GetSpecItem();
	if( pItemProp == NULL )
	{
		return fRate;
	}

	fRate	= pItemProp->nAdjParamVal[ 0 ] / nsMadrigalGift::DIVIDE_POINT_REWARD_RATE;

	if( fRate < 1.0f )
	{
		fRate = 1.0f;
	}

	return fRate;
}

int		FLMadrigalGift::GetAddRewardLevel( FLWSUser* pUser ) const
{
	int nAddLevel	= 0;
	
	if( IsValidObj( pUser ) == FALSE )
	{
		return nAddLevel;
	}

	IBuff* pBuff	= pUser->GetBuffByIk3( IK3_MADRIGALBLESS );
	if( pBuff == NULL )
	{
		return nAddLevel;
	}

	const PT_ITEM_SPEC pItemProp	= pBuff->GetSpecItem();
	if( pItemProp == NULL )
	{
		return nAddLevel;
	}

	nAddLevel	= pItemProp->nAdjParamVal[ 1 ];

	return nAddLevel;
}

bool	FLMadrigalGift::GetGiftStep( const int nHavePoint, nsMadrigalGift::T_GIFT_STEP & kGiftStep ) const
{
	nsMadrigalGift::T_GIFT_POINT kGiftPoint( nHavePoint, nHavePoint );

	nsMadrigalGift::GiftStepDataMap::const_iterator itr = m_mapGiftStepData.find( kGiftPoint );
	if( itr != m_mapGiftStepData.end() )
	{
		kGiftStep.byStep	= itr->second.byStep;
		kGiftStep.nAddLevel	= itr->second.nAddLevel;

		return true;
	}

	return false;
}

bool	FLMadrigalGift::GetGiftPoint( const BYTE byStep, nsMadrigalGift::T_GIFT_POINT & kGiftPoint ) const
{
	nsMadrigalGift::GiftPointDataMap::const_iterator itr = m_mapGiftPointData.find( byStep );
	if( itr != m_mapGiftPointData.end() )
	{
		kGiftPoint.nMinPoint	= itr->second.nMinPoint;
		kGiftPoint.nMaxPoint	= itr->second.nMaxPoint;

		return true;
	}

	return false;
}

bool	FLMadrigalGift::GetRewardItemList( const DWORD dwGrade, const int nLevel, nsMadrigalGift::RewardItemVec & vecRewardItem ) const
{
	vecRewardItem.clear();

	nsMadrigalGift::T_GIFT_PLAYER_DATA kPlayerData( dwGrade, nLevel, nLevel );

	nsMadrigalGift::RewardItem_PlayerMap::const_iterator itr = m_mapRewardItemData.find( kPlayerData );
	if( itr != m_mapRewardItemData.end() )
	{
		vecRewardItem.assign( itr->second.begin(), itr->second.end() );
	}

	return ( vecRewardItem.empty() == false );
}

bool	FLMadrigalGift::GetRewardItem( const nsMadrigalGift::RewardItemVec & vecRewardItem, nsMadrigalGift::T_GIFT_ITEM_DATA & kRewardItem ) const
{
	if( vecRewardItem.empty() == true )
	{
		return false;
	}

	const DWORD dwRand	= ::xRandom( nsMadrigalGift::MAX_REWARD_PROB );

	for( nsMadrigalGift::RewardItemVec::const_iterator pos = vecRewardItem.begin(); pos != vecRewardItem.end(); ++pos )
	{
		const nsMadrigalGift::T_GIFT_ITEM_DATA kGiftItem = *pos;
		if( dwRand < kGiftItem.dwRewardProb )
		{
			kRewardItem = kGiftItem;
			return true;
		}
	}

	return false;
}

void	FLMadrigalGift::OnLogin( FLWSUser* pUser, const time_t tLogOut ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return;
	}

	SendSystemActiveNoti( pUser );

	if( CalcLoginPoint( pUser, tLogOut ) == true )
	{	
		SendGiftPointNoti( pUser );
	}
}

void	FLMadrigalGift::OnKeepConnection( FLWSUser* pUser, const DWORD dwTick ) const
{
	if( IsActivate() == false )
	{
		return;
	}

	if( CalcKeepConnectionPoint( pUser, dwTick ) == true )
	{
		SendGiftPointNoti( pUser );
	}
}

void	FLMadrigalGift::OnMonsterKill( FLWSUser* pUser, CMover* pMover ) const
{
	if( IsActivate() == false )
	{
		return;
	}

	if( CalcMonsterKillPoint( pUser, pMover ) == true )
	{
		SendGiftPointNoti( pUser );
	}
}

void	FLMadrigalGift::OnLevel( FLWSUser* pUser ) const
{
	if( IsActivate() == false )
	{
		return;
	}

	SendGiftPointNoti( pUser );
}

bool	FLMadrigalGift::OnReqReceiveGiftItem( FLWSUser* pUser ) const
{
	if( IsValidObj( pUser ) == FALSE )
	{
		return false;
	}

	if( IsActivate() == false )
	{
		return false;
	}

	const bool bret	= DoReceiveGiftItem( pUser );

	SendReceiveGiftItemAck( pUser );

	return bret;
}

bool	FLMadrigalGift::DoReceiveGiftItem( FLWSUser* pUser ) const
{
	nsMadrigalGift::T_GIFT_STEP kGiftStep;

	if( GetGiftStep( pUser->m_kMadrigalGiftPoint.nPoint, kGiftStep ) == false )
	{
		pUser->AddDefinedText( TID_MMI_MADRIGALGIFT_TEXT09 );
		return false;
	}

	if( kGiftStep.byStep <= 0 )
	{
		pUser->AddDefinedText( TID_MMI_MADRIGALGIFT_TEXT09 );
		return false;
	}

	nsMadrigalGift::RewardItemVec vecRewardItem;

	const DWORD dwGrade = pUser->GetLegendChar();
	const int	nLevel	= pUser->GetLevel() + kGiftStep.nAddLevel + GetAddRewardLevel( pUser );

	if( GetRewardItemList( dwGrade, nLevel, vecRewardItem ) == false )
	{
		pUser->AddDefinedText( TID_MMI_MADRIGALGIFT_TEXT09 );
		return false;
	}

	nsMadrigalGift::T_GIFT_ITEM_DATA kRewardItem;

	if( GetRewardItem( vecRewardItem, kRewardItem ) == false )
	{
		pUser->AddDefinedText( TID_MMI_MADRIGALGIFT_TEXT09 );
		return false;
	}

	const PT_ITEM_SPEC pItemProp = g_xSpecManager->GetSpecItem( kRewardItem.dwItemID );
	if( pItemProp == NULL )
	{
		FLASSERT( 0 );
		return false;
	}

	const int nQuantity	= static_cast<int>( ::xRandom( kRewardItem.dwMinQuantity, kRewardItem.dwMaxQuantity ) );

	if( pItemProp->dwItemKind3 == IK3_GOLD )
	{
		if( pUser->AddGold( nQuantity ) == FALSE )
		{
			FLItemElem kItemElem;
			g_dpDBClient.SendQueryPostMail( pUser->m_idPlayer, NULL_PLAYER_ID, kItemElem, nQuantity, "", "" );
			pUser->AddDefinedText( TID_MMI_PENYAOVERFLOW, "%d", nQuantity );
		}
		else
		{
			pUser->AddDefinedText( TID_MMI_MADRIGALGIFT_TEXT11, "%d", nQuantity );
		}

		g_DPSrvr.PutPenyaLog( pUser, "P", "MADRIGAL_GIFT", -nQuantity );
	}
	else
	{
		FLItemElem kItemElem;
		kItemElem.m_dwItemId	= kRewardItem.dwItemID;
		kItemElem.m_nItemNum	= nQuantity;

		kItemElem.SetSerialNumber();

		if( pUser->CreateItem( &kItemElem ) == FALSE )
		{
			pUser->AddDefinedText( TID_MMI_MADRIGALGIFT_TEXT10 );
			return false;
		}

		pUser->AddDefinedText( TID_MMI_MADRIGALGIFT_TEXT05, "\"%s\"", pItemProp->szName );

		g_DPSrvr.PutItemLog( pUser, "P", "MADRIGAL_GIFT", &kItemElem, nQuantity );
	}

	pUser->m_kMadrigalGiftPoint.nPoint	= 0;

	SendGiftPointNoti( pUser );

	return true;
}
