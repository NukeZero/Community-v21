#include "StdAfx.h"

#include "FLDefinesToString.h"

#include "FLCStyleParser.h"


FLDefinedsToString::FLDefinedsToString()
{
	load();
}

FLDefinedsToString::~FLDefinedsToString()
{

}

FLDefinedsToString & FLDefinedsToString::GetInstance()
{
	static FLDefinedsToString inst;
	return inst;
}

// 숫자로 define 된 것만 읽는다.
void FLDefinedsToString::load()
{
	const char* pszCDefines[] =
	{
		"define.h",
		"defineNeuz.h",
		"defineQuest.h",
		"defineJob.h",
		"defineItem.h",
		"defineWorld.h",
		"defineItemkind.h",
		"lang.h",
		"defineObj.h",
		"defineAttribute.h",
		"defineSkill.h",
		"defineText.h",
		"defineSound.h",
		"resdata.h",
		"WndStyle.h", 
		"definelordskill.h"	//sun: 12, ±º??
		, "defineHonor.h"	//sun: 13, ´Þ??
#ifdef __IMPROVE_MAP_SYSTEM
		, "ContinentDef.h"
		, "defineMapComboBoxData.h"
#endif // __IMPROVE_MAP_SYSTEM
		, "defineEventArena.h"
		, "defineItemGrade.h"
		, "defineItemType.h"
	};

	for( size_t i = 0; i < _countof( pszCDefines ); ++i )
	{
		vector< FLCStyleReader::FLToken > v;
		FLCStyleReader reader( pszCDefines[ i ] );
		RELEASE_ASSERT( reader.Load( v ) == true );

		vector< FLCStyleReader::FLToken >::const_iterator it		= v.begin(); 
		vector< FLCStyleReader::FLToken >::const_iterator it_end	= v.end();
		while( it != it_end )
		{
			const FLCStyleReader::FLToken token	= FLCStyleReader::PopToken( it, it_end );
			if( ::strcmp( token.s.c_str(), "#" ) != 0 )
				continue;

			const FLCStyleReader::FLToken def	= FLCStyleReader::PopToken( it, it_end );
			if( ::strcmp( def.s.c_str(), "define" ) != 0 )
				continue;

			const FLCStyleReader::FLToken str	= FLCStyleReader::PopToken( it, it_end );
			if( str.e != FLCStyleReader::NAME_VALUE )
				continue;
			//if( ::strcmp( str.s.c_str(), "define" ) != 0 )
			//	continue;
			const FLCStyleReader::FLToken value	= FLCStyleReader::GetToken( it, it_end );
			if( value.e != FLCStyleReader::NUMBERIC_VALUE )
				continue;

			FLCStyleReader::PopToken( it, it_end );
			
			if (!m_mapDefineds.insert(make_pair(str.s, std::stoul(value.s))).second)
				FLERROR_LOG( PROGRAM_NAME, _T( "[ DUPLICATE DEFINE : (%s) (%s) ]" ), str.s.c_str(), value.s.c_str() );
			//RELEASE_ASSERT(  );
		}
	}
}

bool FLDefinedsToString::GetInt( int & o_int, const char * pszStr )
{
	map< string, int >::const_iterator it = m_mapDefineds.find( pszStr );
	if( it == m_mapDefineds.end() )
		return false;

	o_int				= it->second;
	return true;
}


