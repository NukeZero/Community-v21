#include "StdAfx.h"

#include "FLConsignmentSale.h"
#include <../../_CommonDefine/util/FLIniFile.h>

//----------------------------------------------------------------------------------------------------------//

FLConsignmentSale::T_USER_DATA::T_USER_DATA() 
: dwLastQueryTick( 0 )
, bRegisting( FALSE )
{

}

DWORD FLConsignmentSale::GetExtendRegistCountMax( FLWSUser & kUser )
{
	return kUser.GetAdjValueDuplicationBuff( IK3_TRADEREGISTERITEM );
}

int FLConsignmentSale::GetTax( FLWSUser* kUser, const int nTotPrice )
{
	//버프와 이벤트는 기획서 나오면 여따 적용
	UINT64 u64Tax	= 0;
	u64Tax	= ( static_cast< UINT64 >( nTotPrice ) * CONSIGNMENT_SALE_SPEC().m().nTaxRate ) / FLConsignmentSale_Spec::TAX_RATE_MAX;

	if( IsValidObj( kUser) == TRUE )
	{
		IBuff* pBuff			= kUser->GetBuffByIk3( IK3_TRADEREGISTERITEM );
		if( pBuff != NULL )
		{
			u64Tax	= 0;
		}
	}
	
	return static_cast< int >( u64Tax );
}


BOOL FLConsignmentSale::IsQueryableTime_TryUpdateTime( T_USER_DATA & kUserData )
{
	if( ::GetTickCount() >= ( kUserData.dwLastQueryTick + CONSIGNMENT_SALE_SPEC().m().nQueryIntervalTick ) )
	{
		kUserData.dwLastQueryTick	= ::GetTickCount();
		return TRUE;
	}

	return FALSE;
}




//----------------------------------------------------------------------------------------------------------//

FLConsignmentSale_Spec::T_STOCK::T_STOCK() 
: nSaleHour( 0 )
, nDefaultRegistLimit( 0 )
, nTaxRate( 0 )
, nQueryIntervalTick( 0 )
{

}

//-----------------------------------------------------------------------------------------//

FLConsignmentSale_Spec::FLConsignmentSale_Spec()
{
	//Load();
}

FLConsignmentSale_Spec::~FLConsignmentSale_Spec()
{

}

FLConsignmentSale_Spec & FLConsignmentSale_Spec::GetInstance()
{
	static FLConsignmentSale_Spec inst;
	return inst;
}

void FLConsignmentSale_Spec::Load()
{
	loadConfig();
	loadVendibleWorldList();
}

BOOL FLConsignmentSale_Spec::IsVendible( const DWORD dwWorldID ) const
{
	WorldIDMap::const_iterator it = m_mapVendibleWorldID.find( dwWorldID );
	return ( it != m_mapVendibleWorldID.end() ) ? TRUE : FALSE;
}


void FLConsignmentSale_Spec::loadConfig()
{
	if( CS_NOT_SUPPORTED == _GetContentState( CT_CONSIGNMENT_MARKET ) )
		return;


	const TCHAR szFileName[] = _T( "ConsignmentMarket\\ConsignmentMarket.ini" );

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

	FLIniFile ini;
	const FLIniFile::E_RESULT eRet = ini.InitializeINIFile( szFullPath, FLIniFile::E_BASE_PATH::ABSOLUTE_PATH );
	if( eRet != FLIniFile::E_OK )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s), ERROR_CODE:(%d) ]" ), szFullPath, eRet );
		RELEASE_BREAK;
	}

	if( ini.IniReadInt( _T( "CONTENT" ), _T( "SALE_HOUR" ), m_kStock.nSaleHour ) == FALSE ||
		ini.IniReadInt( _T( "CONTENT" ), _T( "DEFAULT_REGIST_LIMIT" ), m_kStock.nDefaultRegistLimit ) == FALSE ||
		ini.IniReadInt( _T( "CONTENT" ), _T( "TAX_RATE" ), m_kStock.nTaxRate ) == FALSE ||
		ini.IniReadInt( _T( "SYSTEM" ), _T( "QUERY_INTERVAL_TICK" ), m_kStock.nQueryIntervalTick ) == FALSE
		)
		RELEASE_BREAK;

	RELEASE_ASSERT( m_kStock.nSaleHour > 0 );
	RELEASE_ASSERT( m_kStock.nDefaultRegistLimit >= 0 );
	RELEASE_ASSERT( m_kStock.nTaxRate < TAX_RATE_MAX );
	RELEASE_ASSERT( m_kStock.nQueryIntervalTick >= 0 );
}

void FLConsignmentSale_Spec::loadVendibleWorldList()
{
	//const char * szFileName	= "/ConsignmentMarket/VendibleWorldList.txt";		//실행 폴더 기준

	const char * szFileName	= "\\ConsignmentMarket\\VendibleWorldList.txt";		//실행 폴더 기준

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), szFileName );

// 	char szCurrPath[ MAX_PATH ];
// 	::GetCurrentDirectory( sizeof( szCurrPath ), szCurrPath );
// 	FLStrcat( szCurrPath, _countof( szCurrPath ), szFileName );

	CScript script;
	if( script.Load( szFullPath ) == FALSE )
	//if( script.Load( szFileName ) == FALSE )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return;
	}

	for( DWORD dwWorldID = script.GetNumber(); script.tok != FINISHED; dwWorldID = script.GetNumber() )
	{
		if( dwWorldID == WI_WORLD_NONE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ INVALID WORLDID(%s) ]" ), script.Token );
			continue;
		}

		if( IsVendible( dwWorldID ) == TRUE )
		{
			FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE WORLDID(%d) ]" ), dwWorldID );
			continue;
		}

		m_mapVendibleWorldID.insert( WorldIDMap::value_type( dwWorldID, dwWorldID ) );
	}
}

const FLConsignmentSale_Spec::T_STOCK & FLConsignmentSale_Spec::m() const
{
	return m_kStock;
}





