
#include "FLCmdOption.h"


FLCmdOption::FLCmdOption()
{
}

FLCmdOption::~FLCmdOption()
{
	m_vecDefinedOptions.clear();
	m_vecOptions.clear();
}

void	FLCmdOption::AddOption( const tstring& strDefinedOption )
{
	if( !IsDefinedOption( strDefinedOption ) )
	{
		m_vecDefinedOptions.push_back( strDefinedOption );
	}
}

void	FLCmdOption::AddOptions( const std::vector< tstring >& vecDefinedOptions )
{
	std::vector< tstring >::const_iterator pos = vecDefinedOptions.begin();
	std::vector< tstring >::const_iterator end = vecDefinedOptions.end();

	for( ; pos != end; ++pos )
	{
		AddOption( *pos );
	}
}

bool	FLCmdOption::AddValue( const tstring& strName, const tstring& strValue )
{
	if( IsDefinedOption( strName ) )
	{
		T_OPTION* pOption = Find( strName );
		if( !pOption )
		{
			T_OPTION tOption;
			tOption.strName = strName;
			m_vecOptions.push_back( tOption );

			pOption = Find( strName );
		}

		if( !pOption )
		{
			return false;
		}

		if( !strValue.empty() )
		{
			pOption->vecValues.push_back( strValue );
		}

		return true;
	}

	return false;
}

bool	FLCmdOption::IsSupplied( const tstring& strName )	const
{
	if( Find( strName ) )
	{
		return true;
	}

	return false;
}

tstring	FLCmdOption::GetFirstValue( const tstring& strName )	const
{
	const T_OPTION* pOption = Find( strName );
	if( pOption )
	{
		if( !pOption->vecValues.empty() )
		{
			return pOption->vecValues.front();
		}
	}

	return tstring( _T( "" ) );
}

bool	FLCmdOption::GetValues( const tstring& strName, std::vector< tstring >& vecValues )	const
{
	const T_OPTION* pOption = Find( strName );
	if( pOption )
	{
		vecValues = pOption->vecValues;
		return true;
	}

	return false;
}

bool	FLCmdOption::IsDefinedOption( const tstring& strName )	const
{
	DEFINED_OPTION_LIST::const_iterator pos = m_vecDefinedOptions.begin();
	DEFINED_OPTION_LIST::const_iterator end = m_vecDefinedOptions.end();

	for( ; pos != end; ++pos )
	{
		if( strName == *pos )
		{
			return true;
		}
	}

	return false;
}

const FLCmdOption::T_OPTION*	FLCmdOption::Find( const tstring& strName )	const
{
	const T_OPTION* pOption = NULL;

	OPTION_LIST::const_iterator pos = m_vecOptions.begin();
	OPTION_LIST::const_iterator end = m_vecOptions.end();
	for( ; pos != end; ++pos )
	{
		const T_OPTION& tOption = (*pos);
		if( tOption.strName == strName )
		{
			pOption = &tOption;
			break;
		}
	}

	return pOption;
}

FLCmdOption::T_OPTION*	FLCmdOption::Find( const tstring& strName )
{
	T_OPTION* pOption = NULL;

	OPTION_LIST::iterator pos = m_vecOptions.begin();
	OPTION_LIST::iterator end = m_vecOptions.end();
	for( ; pos != end; ++pos )
	{
		T_OPTION& tOption = (*pos);
		if( tOption.strName == strName )
		{
			pOption = &tOption;
			break;
		}
	}

	return pOption;
}
