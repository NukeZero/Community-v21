#include "StdAfx.h"

#include "FLCooperativeContributions.h"
#include "../_CommonDefine/FLPacketStruct.h"

#include "FLCC_Condition.h"
#include "FLCC_Reward.h"


namespace nsCooperativeContributions {


FLCooperativeContributions::FLCooperativeContributions()
{
}

FLCooperativeContributions::~FLCooperativeContributions()
{
	for( ContributionMap::iterator kItr = m_mapContribution.begin(); kItr != m_mapContribution.end(); ++kItr )
	{
		SAFE_DELETE( kItr->second );
	}
	m_mapContribution.clear();
}

void FLCooperativeContributions::Send_ContributionInfo( FLWSUser & User )
{
	for( ContributionMap::const_iterator it = m_mapContribution.begin(); it != m_mapContribution.end(); ++it )
	{
		FLContribution & contribution	= *it->second;

		char szMessage[ 2048 ] = { 0 };

		FLSPrintf( szMessage, _countof( szMessage ), "[ ID(%u), IsComplete(%s), start(%s), end(%s) ]"
			, contribution.m_dwID
			, ( contribution.IsExpired() == TRUE ) ? 
				"UNKNOWN" : 
				( contribution.IsComplete() ) ? 
					"COMPLETE" : 
					"NOT COMPLETE"
			, FLTmToString( contribution.m_tmStartDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()
			, FLTmToString( contribution.m_tmEndDate, FLTm::E_FORMAT::YYYYMMDD_HHMMSS ).c_str()
			);

		g_DPCoreClient.SendSetSnoop( User.m_idPlayer, User.m_idPlayer, FALSE );
		g_DPCoreClient.SendChat( User.m_idPlayer, User.m_idPlayer, szMessage );
		g_DPCoreClient.SendSetSnoop( User.m_idPlayer, User.m_idPlayer, TRUE );
	}
}

void FLCooperativeContributions::TryReward_LoginUser( FLWSUser & User )
{
	for( ContributionMap::const_iterator it = m_mapContribution.begin(); it != m_mapContribution.end(); ++it )
	{
		FLContribution & contribution	= *it->second;

		if( contribution.IsComplete() == FALSE || contribution.IsEventPeriod() == FALSE )
			continue;
		
		//글로벌 보상 정보 통보
		typedef		FLContribution::GlobalRewardVec		GlobalVec;
		for( GlobalVec::const_iterator itGlobal = contribution.m_vecReward_Global.begin(); itGlobal != contribution.m_vecReward_Global.end(); ++itGlobal )
		{
			//IGlobalReward * p = *itGlobal;
			//p->RewardNotify( User );
		}
		
		//개인 보상
		for( FLContribution::UserRewardVec::iterator itUserReward = contribution.m_vecReward_User.begin(); itUserReward != contribution.m_vecReward_User.end(); ++itUserReward )
		{
			IUserReward * p = *itUserReward;
			p->TryReward( contribution.m_dwID, User );
		}
	}
}



void FLCooperativeContributions::TryClean_ExpiredGlobalReward()
{
	for( ContributionMap::const_iterator it	= m_mapContribution.begin(); it != m_mapContribution.end(); ++it )
	{
		FLContribution * pContribution		= it->second;
		if( pContribution == NULL )
			continue;

		if( pContribution->IsComplete() == FALSE )
			continue;

		typedef FLContribution::GlobalRewardVec		GlobalVec;
		for( GlobalVec::const_iterator it_reward = pContribution->m_vecReward_Global.begin(); it_reward != pContribution->m_vecReward_Global.end(); ++it_reward )
		{
			IGlobalReward * p	= *it_reward;
			if( pContribution->IsExpired() == TRUE )
				p->Clean_ExpiredReward();
		}
	}
}

FLContribution *	FLCooperativeContributions::FindContribution( const DWORD nContributionID )
{
	ContributionMap::const_iterator it	= m_mapContribution.find( nContributionID );
	return ( it == m_mapContribution.end() ) ? NULL : it->second;
}



//---------------------------------------------------------------------------------------------------------------//




BOOL	FLCooperativeContributions::Query_GetContributedInfo(/* const FLContribution & contribution */)
{
	for( ContributionMap::const_iterator it = m_mapContribution.begin(); it != m_mapContribution.end(); ++it )
	{
		FLContribution & contribution= *it->second;

		const FLTm & tmCurr			= DATE_TIMER().GetCurrentTime();
		if( contribution.m_tmStartDate > tmCurr || contribution.m_tmEndDate < tmCurr )
			continue;

		T_W2DB_PACKET_COOPERATIVE_CONTRIBUTIONS_GET_INFO_REQ req = { 0 };

		req.dwContributionID		= contribution.m_dwID;
		req.tmStart					= contribution.m_tmStartDate;
		req.tmEnd					= contribution.m_tmEndDate;

		BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_CONTRIBUTED_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
		ar.Write( &req, sizeof( req ) );
		SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
	}
	return TRUE;
}

BOOL	FLCooperativeContributions::Query_GetRanking( const u_long idPlayer, const FLContribution & contribution )
{
	T_W2DB_PACKET_CONTRIBUTION_GET_RANKING_REQ req = { 0 };

	req.idPlayer				= idPlayer;
	req.dwContributionID		= contribution.m_dwID;
	req.tmStart					= contribution.m_tmStartDate;
	req.tmEnd					= contribution.m_tmEndDate;

	BEFORESENDDUAL( ar, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_GET_RANKING_REQ, DPID_UNKNOWN, DPID_UNKNOWN );
	ar.Write( &req, sizeof( req ) );
	SEND( ar, &g_dpDBClient, DPID_SERVERPLAYER );	
	return TRUE;
}


BOOL	FLCooperativeContributions::Query_TryWriteRewardInfo()
{
	for( ContributionMap::const_iterator it = m_mapContribution.begin(); it != m_mapContribution.end(); ++it )
	{
		FLContribution & contribution= *it->second;

		const FLTm & tmCurr			= DATE_TIMER().GetCurrentTime();
		if( contribution.m_tmStartDate > tmCurr || contribution.m_tmEndDate < tmCurr )
			continue;

		for( FLContribution::UserRewardVec::iterator it = contribution.m_vecReward_User.begin(); it != contribution.m_vecReward_User.end();
			++it )
		{
			IUserReward * p		= *it;
			p->Query_TryWriteRewardInfo( contribution.m_dwID, contribution.m_tmStartDate, contribution.m_tmEndDate );
		}
	}
	return TRUE;
}

//-------------------------------------------------------------------------------------------------------------------------------//


void FLCooperativeContributions::Load( const BYTE * pBuffer, const int nBufferSize )
{
	CScript script;
	if( script.LoadMemory( pBuffer, nBufferSize ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID SPEC : CAN'T LOAD ]" ) );
		return;
	}
	// 05
	if ( m_mapContribution.size() )
		m_mapContribution.clear();

	while( script.tok != FINISHED )
	{
		// ContributionID
		script.GetToken();
		if( script.Token == "" )
			continue;

		const DWORD dwContributionID = ::atoi( script.Token );//script.GetNumber();
		if( dwContributionID <= 0 )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID CONTRIBUTION ID(%u) ]" ), dwContributionID );
			RELEASE_BREAK;
		}

		ContributionMap::const_iterator it = m_mapContribution.find( dwContributionID );
		if( it != m_mapContribution.end() )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE CONTRIBUTION ID(%u) ]" ), dwContributionID );
			RELEASE_BREAK;			
		}

		script.GetToken();		//skip next
		if( script.Token != "{" )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID TOKEN : pos({) (%s) ]" ), script.Token );
			return;
		}

		// read data
		FLContribution * pContribution  = new FLContribution( dwContributionID );
        for( ;; )
		{
			//완료 }
			script.GetToken();		//skip next
			if( script.Token == "}" )
				break;

			if( script.Token == "SetTitle" )
			{
				script.GetToken();											//"("
				RELEASE_ASSERT( script.Token == "(" );

				pContribution->m_dwDescription_TextID = script.GetNumber();

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );
			}
			else if( script.Token == "SetEndConditionTitle" )
			{	
				script.GetToken();											//"("
				RELEASE_ASSERT( script.Token == "(" );

				pContribution->m_dwEndCond_TextID = script.GetNumber();

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );
			}
			else if( script.Token == "SetNPC")
			{
				script.GetToken();											//"("
				RELEASE_ASSERT( script.Token == "(" );

				script.GetToken();
				char szCharacterKey[ 512 ] = { 0 };
				FLStrcpy( szCharacterKey, _countof( szCharacterKey ), script.token );

				LPCHARACTER pCharacter = prj.GetCharacter( szCharacterKey );
				if( pCharacter == NULL )  
				{
					//VMessageBox( MB_OK, _T( "[ NOT EXIST or CAN'T LOADED NPC(%s) -  character : szCharacterKey ]" ), szCharacterKey );
					FLERROR_LOG( PROGRAM_NAME, _T( "[ NOT EXIST or CAN'T LOADED NPC(%s) -  character : szCharacterKey ]" ), szCharacterKey );
					RELEASE_BREAK;
					return;
				}

				if( pCharacter->m_dwContributionID != 0 )
				{
					//VMessageBox( MB_OK, _T( "[ DUPLICATE CHARACTER - szCharacterKey(%s), dwContributionID(%u) ]" ), szCharacterKey, dwContributionID );
					FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE CHARACTER - szCharacterKey(%s), dwContributionID(%u) ]" ), szCharacterKey, dwContributionID );
					//RELEASE_BREAK;
					//return;	//05
				}

				pCharacter->m_dwContributionID	= dwContributionID;

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );
			}

			//		//기간
			else if( script.Token == "SetPeriod" )			//::GetLanguage() 
			{
				script.GetToken();											//"("
				RELEASE_ASSERT( script.Token == "(" );

				const int iLang		= script.GetNumber();					//LANG_KOR

				script.GetToken();	script.Token;							//","
				script.GetToken();	
				const std::string strStartDate = script.Token;

				const FLTm sDate	= FLStringToFLTm( strStartDate.c_str(), ( strStartDate.size() == FLTm::DATETIME_STRING_MAX - 1 ) ? FLTm::E_FORMAT::YYYYMMDD_HHMMSS : FLTm::E_FORMAT::YYYYMMDD );		//start date
				if( sDate == INVALID_DATETIME )
				{
					//VMessageBox( MB_OK, _T( "[ INVALID DATA : startDate(%s) ]" ), script.Token );
					FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATA : startDate(%s) ]" ), script.Token );
					RELEASE_BREAK;
				}

				script.GetToken();	script.Token;							//","
				script.GetToken();	
				const std::string strEndDate = script.Token;

				FLTm eDate			= FLStringToFLTm( strEndDate.c_str(), ( strEndDate.size() == FLTm::DATETIME_STRING_MAX - 1 ) ? FLTm::E_FORMAT::YYYYMMDD_HHMMSS : FLTm::E_FORMAT::YYYYMMDD );	//end date
				if( eDate == INVALID_DATETIME )
				{
					//VMessageBox( MB_OK, _T( "[ INVALID DATA : endDate(%s) ]" ), script.Token );
					FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATA : endDate(%s) ]" ), script.Token );
					RELEASE_BREAK;
				}

				if( strEndDate.size() == ( FLTm::DATE_STRING_MAX - 1 ) )		// 날짜만 입력의 경우 하루를 더해준다 //해당 날짜 까지 이벤트가 되어야 하므로 1 DAY를 더해준다.
				{
					if( FLAddDateTime( eDate, FLTm::ONE_DAY_SECOND ) == FALSE )							
					{
						//VMessageBox( MB_OK, _T( "[ INVALID DATA : endDate 2(%s) ]" ), script.Token );
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID DATA : endDate 2(%s) ]" ), script.Token );
						RELEASE_BREAK;
					}
				}

				if( sDate >= eDate )
				{
					//VMessageBox( MB_OK, _T( "[ INVALID PERIOD DATA: ContributionID(%u) startDate(%s) endDate(%s) ]" )
					//	, dwContributionID, FLTmToString( sDate, FLTm::E_FORMAT::YYYYMMDD ).c_str(), FLTmToString( eDate, FLTm::E_FORMAT::YYYYMMDD ).c_str() );
					FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PERIOD DATA: ContributionID(%u) startDate(%s) endDate(%s) ]" )
						, dwContributionID, FLTmToString( sDate, FLTm::E_FORMAT::YYYYMMDD ).c_str(), FLTmToString( eDate, FLTm::E_FORMAT::YYYYMMDD ).c_str() );
					RELEASE_BREAK;
				}

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				// set period
				if( iLang == g_xFlyffConfig->GetMainLanguage() ) 
				{
					if( pContribution->m_tmStartDate != INVALID_DATETIME || pContribution->m_tmEndDate != INVALID_DATETIME )
					{
						//VMessageBox( MB_OK, _T( "[ DUPLICATE PERIOD : ContributionID(%u) ]" ), dwContributionID );
						FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE PERIOD : ContributionID(%u) ]" ), dwContributionID );
						RELEASE_BREAK;
					}
					
					pContribution->m_tmStartDate	= sDate;
					pContribution->m_tmEndDate		= eDate;

					//--------------------------------------------- 검증 ---------------------------------------//
					if( pContribution->m_tmStartDate >= pContribution->m_tmEndDate/*pContribution->IsValid_EndCond() == FALSE*/ )
					{
//						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID CONTRIBUTION INFO (%u) ]" ), pContribution->m_dwID );
						FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID PERIOD. CONTRIBUTION_ID:(%u) ]" ), pContribution->m_dwID );
						RELEASE_BREAK;
					}
				}
			}

			//---------------------------------------- 유저 기부 조건 ---------------------------------------//
			else if( script.Token == "SetUserCondLevel" )
			{
				script.GetToken();												//"("
				RELEASE_ASSERT( script.Token == "(" );

				const BYTE byMin		= static_cast< BYTE >( script.GetNumber() );					//Min
				script.GetToken();	script.Token;								//","
				const BYTE byMax		= static_cast< BYTE >( script.GetNumber() );				//Max

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				pContribution->m_vecBeginCondition_User.push_back( new FLLevelBeginConditioin_User( byMin, byMax ) );
			}
			else if( script.Token == "SetUserCondJob" )
			{
				script.GetToken();												//"("
				RELEASE_ASSERT( script.Token == "(" );

				FLJobBeginCondition_User * pCondition = new FLJobBeginCondition_User;
				do
				{
					const int nJob				= script.GetNumber();
					pCondition->RegistJob( nJob );
					script.GetToken(); // , or )
				} while( *script.token != ')'  );

				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				pContribution->m_vecBeginCondition_User.push_back( pCondition );
			}

			//---------------------------------------- 글로벌 조건 ---------------------------------------//
			else if( script.Token == "SetEndCondItem" )
			{
				script.GetToken();												//"("
				RELEASE_ASSERT( script.Token == "(" );


				const DWORD dwTextID			= script.GetNumber();			// TextID

				script.GetToken();	script.Token;								//","
				const DWORD dwItemID			= script.GetNumber();					//ItemID

				script.GetToken();	script.Token;								//","
				const int nItemContributionUnit	= script.GetNumber();					//Unit
				RELEASE_ASSERT( nItemContributionUnit > 0 );

				script.GetToken();	script.Token;								//","
				const DWORD dwItemConditionMax	= script.GetNumber();					//Max

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				RELEASE_ASSERT( nItemContributionUnit < ( int )dwItemConditionMax );

				pContribution->m_vecGlobalCondition.push_back( new FLItemEndCondition_Global( dwTextID, dwItemID, dwItemConditionMax, nItemContributionUnit ) );
			}
			else if( script.Token == "SetEndCondGold" )
			{
				script.GetToken();												//"("
				RELEASE_ASSERT( script.Token == "(" );

				const DWORD dwTextID		= script.GetNumber();			// TextID
				script.GetToken();	script.Token;								//","

				const int nContributionUnit	= script.GetNumber();					//Unit
				RELEASE_ASSERT( nContributionUnit > 0 );
				script.GetToken();	script.Token;								//","

				const int nGoldMax			= script.GetNumber();					//Min
				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				RELEASE_ASSERT( nContributionUnit < nGoldMax );

				pContribution->m_vecGlobalCondition.push_back( new FLGoldEndCondition_Global( dwTextID, nGoldMax, nContributionUnit ) );
			}

			//-------------------------------------- 보상 설정 -------------------------------------//
			else if( script.Token == "SetRewardItem" )
			{
				script.GetToken();												//"("
				RELEASE_ASSERT( script.Token == "(" );

				const DWORD dwItemID				= script.GetNumber();					//Min
				script.GetToken();	script.Token;								//","
				const short shRewardCount			= static_cast< short >( script.GetNumber() );					//Max
				script.GetToken();	script.Token;								//","
				const DWORD dwRewardTextID			= script.GetNumber();

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				pContribution->m_vecReward_User.push_back( new FLItemReward_User( dwItemID, shRewardCount, dwRewardTextID ) );
			}
			else if( script.Token == "SetRewardExpRate" || script.Token == "SetRewardDropRate" 
				|| script.Token == "SetRewardShopBuyFactor" || script.Token == "SetRewardShopSellFactor" )
			{
				const CString strFunction	= script.Token;

				script.GetToken();												//"("
				RELEASE_ASSERT( script.Token == "(" );

				//const float fRate					= script.GetFloat();
				const DWORD	dwRate					= script.GetNumber();
				script.GetToken();	script.Token;								//","
				const DWORD dwRewardTextID			= script.GetNumber();

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				if( strFunction == "SetRewardExpRate" )
					pContribution->m_vecReward_Global.push_back( new FLReward_Global< DWORD >( GLOBAL_REWARD_ALARM_EXP_FACTOR(), dwRate, dwRewardTextID ) );
				else if( strFunction == "SetRewardDropRate" )
					pContribution->m_vecReward_Global.push_back( new FLReward_Global< DWORD >( GLOBAL_REWARD_ALARM_DROP_RATE(), dwRate, dwRewardTextID ) );
				else if( strFunction == "SetRewardShopBuyFactor" )
					pContribution->m_vecReward_Global.push_back( new FLReward_Global< DWORD >( GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR(), dwRate, dwRewardTextID ) );
				else if( strFunction == "SetRewardShopSellFactor" )
					pContribution->m_vecReward_Global.push_back( new FLReward_Global< DWORD >( GLOBAL_REWARD_ALARM_SHOP_SELL_FACTOR(), dwRate, dwRewardTextID ) );

				//if( strFunction == "SetRewardExpRate" )
				//	pContribution->m_vecReward_Global.push_back( new FLReward_Global< float >( GLOBAL_REWARD_ALARM_EXP_FACTOR(), fRate, dwRewardTextID ) );
				//else if( strFunction == "SetRewardDropRate" )
				//	pContribution->m_vecReward_Global.push_back( new FLReward_Global< float >( GLOBAL_REWARD_ALARM_DROP_RATE(), fRate, dwRewardTextID ) );
				//else if( strFunction == "SetRewardShopBuyFactor" )
				//	pContribution->m_vecReward_Global.push_back( new FLReward_Global< float >( GLOBAL_REWARD_ALARM_SHOP_BUY_FACTOR(), fRate, dwRewardTextID ) );
				//else if( strFunction == "SetRewardShopSellFactor" )
				//	pContribution->m_vecReward_Global.push_back( new FLReward_Global< float >( GLOBAL_REWARD_ALARM_SHOP_SELL_FACTOR(), fRate, dwRewardTextID ) );
			}
			else if( script.Token == "SetRewardDefencePower" || script.Token == "SetRewardAttackPower" )
			{
				const CString strFunction	= script.Token;

				script.GetToken();												//"("
				RELEASE_ASSERT( script.Token == "(" );

				const int nPower			= script.GetNumber();
				script.GetToken();	script.Token;								//","
				const DWORD dwRewardTextID	= script.GetNumber();

				script.GetToken();												//")"
				RELEASE_ASSERT( script.Token == ")" );
				script.GetToken();												//";"
				RELEASE_ASSERT( script.Token == ";" );

				if( strFunction == "SetRewardDefencePower" )
					pContribution->m_vecReward_Global.push_back( new FLReward_Global< int >( GLOBAL_REWARD_ALARM_DEFENCE_POWER(), nPower, dwRewardTextID ) );
				else if( strFunction == "SetRewardAttackPower" )
					pContribution->m_vecReward_Global.push_back( new FLReward_Global< int >( GLOBAL_REWARD_ALARM_ATTACK_POWER(), nPower, dwRewardTextID ) );
			}
			else
			{
				//VMessageBox( MB_OK, _T( "[ INVALID TOKEN(%s) ]" ), script.Token );
				FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID TOKEN(%s) ]" ), script.Token );
				RELEASE_BREAK;
			}
		}

		//--------------------------------------------- 검증 ---------------------------------------//
// 		if( pContribution->m_tmStartDate >= pContribution->m_tmEndDate/*pContribution->IsValid_EndCond() == FALSE*/ )
// 		{
// 			//VMessageBox( MB_OK, _T( "[ INVALID CONTRIBUTION INFO (%u) ]" ), pContribution->m_dwID );
// 			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID CONTRIBUTION INFO (%u) ]" ), pContribution->m_dwID );
// 			RELEASE_BREAK;
// 		}

		//////////////////////////////////////////////////////////////////////////
		/*
		국가설정에 대한 정보는 저장되지 않고있다.
		국가설정이 일치하면 시작시간과 종료시간이 셋팅되므로 시작시간과 종료시간이 디폴트면 국가설정이 불일치 이므로 삭제해도 무방할듯..
		*/
		if( pContribution->m_tmStartDate == INVALID_DATETIME || pContribution->m_tmEndDate == INVALID_DATETIME )
		{
			FLINFO_LOG( PROGRAM_NAME, _T( "[ Different Country Event. ID:(%u) ]" ), pContribution->m_dwID );
			SAFE_DELETE( pContribution );
		}
		else
		{
			if( false == m_mapContribution.insert( ContributionMap::value_type( pContribution->m_dwID, pContribution ) ).second )
			{
				SAFE_DELETE( pContribution );
			}
		}
		//////////////////////////////////////////////////////////////////////////
	}
}

FLCooperativeContributions & FLCooperativeContributions::GetInstance()
{
	static FLCooperativeContributions inst;
	return inst;
}

} //nsCooperativeContributions







