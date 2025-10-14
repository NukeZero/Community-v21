
#include "StdAfx.h"
#include "FLRandomOption.h"


FLRandomOption::FLRandomOption()
{
	Clear();
}

FLRandomOption::FLRandomOption( const FLRandomOption & rhs )
{
	Clear();
	*this	= rhs;
}

FLRandomOption::~FLRandomOption()
{
	Clear();
}

FLRandomOption & FLRandomOption::operator = ( const FLRandomOption & kRandomOption )
{
	m_wRandomOptionExtensionFlag	= kRandomOption.m_wRandomOptionExtensionFlag;

	m_vecRandomOptionExtension.assign( kRandomOption.m_vecRandomOptionExtension.begin(), kRandomOption.m_vecRandomOptionExtension.end() );
	m_vecNewRandomOptionExtension.assign( kRandomOption.m_vecNewRandomOptionExtension.begin(), kRandomOption.m_vecNewRandomOptionExtension.end() );
	m_vecCombinedRandomOptionExtension.assign( kRandomOption.m_vecCombinedRandomOptionExtension.begin(), kRandomOption.m_vecCombinedRandomOptionExtension.end() );

	return *this;
}

void	FLRandomOption::Clear()
{
	m_wRandomOptionExtensionFlag	= 0;

	m_vecRandomOptionExtension.clear();
	m_vecNewRandomOptionExtension.clear();
	m_vecCombinedRandomOptionExtension.clear();
}

void	FLRandomOption::InitializeRandomOptionExtension()
{
	m_vecRandomOptionExtension.clear();
}

void	FLRandomOption::InitializeNewRandomOptionExtension()
{
	m_vecNewRandomOptionExtension.clear();
}

void	FLRandomOption::InitializeCombinedRandomOptionExtension()
{
	m_vecCombinedRandomOptionExtension.clear();
}

WORD	FLRandomOption::GetRandomOptionExtensionFlag() const
{
	return m_wRandomOptionExtensionFlag;
}

void	FLRandomOption::SetRandomOptionExtensionFlag( const WORD wRandomOptionExtensionFlag )
{
	m_wRandomOptionExtensionFlag |= wRandomOptionExtensionFlag;
}

bool	FLRandomOption::IsSetSafeGuardRandomOptionExtensionFlag() const
{
	if( ( m_wRandomOptionExtensionFlag & RO_FLAG_SAFEGUARD ) == RO_FLAG_SAFEGUARD )
	{
		return true;
	}

	return false;
}

void	FLRandomOption::SetSafeGuardRandomOptionExtensionFlag()
{
	m_wRandomOptionExtensionFlag |= RO_FLAG_SAFEGUARD;
}

void	FLRandomOption::ResetSafeGuardRandomOptionExtensionFlag()
{
	m_wRandomOptionExtensionFlag &= ~RO_FLAG_SAFEGUARD;
}

bool	FLRandomOption::IsSetRandomOptionExtension() const
{
	if( m_vecRandomOptionExtension.empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLRandomOption::IsSetNewRandomOptionExtension() const
{
	if( m_vecNewRandomOptionExtension.empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLRandomOption::IsSetCombinedRandomOptionExtension() const
{
	if( m_vecCombinedRandomOptionExtension.empty() == false )
	{
		return true;
	}

	return false;
}

size_t	FLRandomOption::GetRandomOptionExtensionSize() const
{
	if( m_vecRandomOptionExtension.empty() == false )
	{
		return m_vecRandomOptionExtension.size();
	}

	return 0;
}

size_t	FLRandomOption::GetNewRandomOptionExtensionSize() const
{
	if( m_vecNewRandomOptionExtension.empty() == false )
	{
		return m_vecNewRandomOptionExtension.size();
	}

	return 0;
}

size_t	FLRandomOption::GetCombinedRandomOptionExtensionSize() const
{
	if( m_vecCombinedRandomOptionExtension.empty() == false )
	{
		return m_vecCombinedRandomOptionExtension.size();
	}

	return 0;
}

WORD	FLRandomOption::GetRandomOptionExtensionDstID( const size_t Nth ) const
{
	if( m_vecRandomOptionExtension.empty() == false && m_vecRandomOptionExtension.size() > Nth )
	{
		return m_vecRandomOptionExtension.at( Nth ).wDstID;
	}

	return 0;
}

WORD	FLRandomOption::GetNewRandomOptionExtensionDstID( const size_t Nth ) const
{
	if( m_vecNewRandomOptionExtension.empty() == false && m_vecNewRandomOptionExtension.size() > Nth )
	{
		return m_vecNewRandomOptionExtension.at( Nth ).wDstID;
	}

	return 0;
}

WORD	FLRandomOption::GetCombinedRandomOptionExtensionDstID( const size_t Nth ) const
{
	if( m_vecCombinedRandomOptionExtension.empty() == false && m_vecCombinedRandomOptionExtension.size() > Nth )
	{
		return m_vecCombinedRandomOptionExtension.at( Nth ).wDstID;
	}

	return 0;
}

short	FLRandomOption::GetRandomOptionExtensionAdjValue( const size_t Nth ) const
{
	if( m_vecRandomOptionExtension.empty() == false && m_vecRandomOptionExtension.size() > Nth )
	{
		return m_vecRandomOptionExtension.at( Nth ).shAdjValue;
	}

	return 0;
}

short	FLRandomOption::GetNewRandomOptionExtensionAdjValue( const size_t Nth ) const
{
	if( m_vecNewRandomOptionExtension.empty() == false && m_vecNewRandomOptionExtension.size() > Nth )
	{
		return m_vecNewRandomOptionExtension.at( Nth ).shAdjValue;
	}

	return 0;
}

short	FLRandomOption::GetCombinedRandomOptionExtensionAdjValue( const size_t Nth ) const
{
	if( m_vecCombinedRandomOptionExtension.empty() == false && m_vecCombinedRandomOptionExtension.size() > Nth )
	{
		return m_vecCombinedRandomOptionExtension.at( Nth ).shAdjValue;
	}

	return 0;
}

bool	FLRandomOption::SetRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension )
{
	if( kRandomOptionExtension.wDstID == 0 || kRandomOptionExtension.shAdjValue == 0 )
	{
		return false;
	}

	if( m_vecRandomOptionExtension.size() >= MAX_RANDOMOPTION_GENERAL_GENERATE_SIZE )
	{
		return false;
	}

	kRandomOptionExtension.byIndex	= static_cast<BYTE>( m_vecRandomOptionExtension.size() );

	m_vecRandomOptionExtension.push_back( kRandomOptionExtension );

	return true;
}

bool	FLRandomOption::SetNewRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension )
{
	if( kRandomOptionExtension.wDstID == 0 || kRandomOptionExtension.shAdjValue == 0 )
	{
		return false;
	}

	if( m_vecNewRandomOptionExtension.size() >= MAX_RANDOMOPTION_GENERAL_GENERATE_SIZE )
	{
		return false;
	}

	kRandomOptionExtension.byIndex	= static_cast<BYTE>( m_vecNewRandomOptionExtension.size() );

	m_vecNewRandomOptionExtension.push_back( kRandomOptionExtension );

	return true;
}

bool	FLRandomOption::SetCombinedRandomOptionExtension( const T_RANDOMOPTION_EXT & kRandomOptionExtension )
{
	if( kRandomOptionExtension.wDstID == 0 || kRandomOptionExtension.shAdjValue == 0 )
	{
		return false;
	}

	if( m_vecCombinedRandomOptionExtension.size() >= MAX_RANDOMOPTION_GENERAL_GENERATE_SIZE )
	{
		return false;
	}

	kRandomOptionExtension.byIndex	= static_cast<BYTE>( m_vecCombinedRandomOptionExtension.size() );

	m_vecCombinedRandomOptionExtension.push_back( kRandomOptionExtension );

	return true;
}

bool	FLRandomOption::OverwriteNewRandomOptionExtension()
{
	if( m_vecRandomOptionExtension.empty() == false && m_vecNewRandomOptionExtension.empty() == false )
	{
		m_vecRandomOptionExtension.assign( m_vecNewRandomOptionExtension.begin(), m_vecNewRandomOptionExtension.end() );

		return true;
	}

	return false;
}

void	FLRandomOption::SerializeRandomOptionExtension( CAr & ar ) const
{
	ar << m_wRandomOptionExtensionFlag;

	if( m_vecRandomOptionExtension.empty() == true )
	{
		ar << static_cast<size_t>( 0 );
	}
	else
	{
		size_t nSize = m_vecRandomOptionExtension.size();

		ar << nSize;

		for( size_t i = 0; i < nSize; ++i )
		{
			T_RANDOMOPTION_EXT kRandomOption = m_vecRandomOptionExtension.at( i );
			ar.Write( &kRandomOption, sizeof( kRandomOption ) );
		}
	}
}

void	FLRandomOption::SerializeNewRandomOptionExtension( CAr & ar ) const
{
	if( m_vecNewRandomOptionExtension.empty() == true )
	{
		ar << static_cast<size_t>( 0 );
	}
	else
	{
		size_t nSize = m_vecNewRandomOptionExtension.size();

		ar << nSize;

		for( size_t i = 0; i < nSize; ++i )
		{
			T_RANDOMOPTION_EXT kRandomOption = m_vecNewRandomOptionExtension.at( i );
			ar.Write( &kRandomOption, sizeof( kRandomOption ) );
		}
	}
}

void	FLRandomOption::SerializeCombinedRandomOptionExtension( CAr & ar ) const
{
	if( m_vecCombinedRandomOptionExtension.empty() == true )
	{
		ar << static_cast<size_t>( 0 );
	}
	else
	{
		size_t nSize = m_vecCombinedRandomOptionExtension.size();

		ar << nSize;

		for( size_t i = 0; i < nSize; ++i )
		{
			T_RANDOMOPTION_EXT kRandomOption = m_vecCombinedRandomOptionExtension.at( i );
			ar.Write( &kRandomOption, sizeof( kRandomOption ) );
		}
	}
}

void	FLRandomOption::DeserializeRandomOptionExtension( CAr & ar )
{
	ar >> m_wRandomOptionExtensionFlag;

	size_t nSize = 0;
	ar >> nSize;

	m_vecRandomOptionExtension.clear();

	for( size_t i = 0; i < nSize; ++i )
	{
		T_RANDOMOPTION_EXT kRandomOption;

		ar.Read( &kRandomOption, sizeof( kRandomOption ) );
		m_vecRandomOptionExtension.push_back( kRandomOption );
	}
}

void	FLRandomOption::DeserializeNewRandomOptionExtension( CAr & ar )
{
	size_t nSize = 0;
	ar >> nSize;

	m_vecNewRandomOptionExtension.clear();

	for( size_t i = 0; i < nSize; ++i )
	{
		T_RANDOMOPTION_EXT kRandomOption;

		ar.Read( &kRandomOption, sizeof( kRandomOption ) );
		m_vecNewRandomOptionExtension.push_back( kRandomOption );
	}
}

void	FLRandomOption::DeserializeCombinedRandomOptionExtension( CAr & ar )
{
	size_t nSize = 0;
	ar >> nSize;

	m_vecCombinedRandomOptionExtension.clear();

	for( size_t i = 0; i < nSize; ++i )
	{
		T_RANDOMOPTION_EXT kRandomOption;

		ar.Read( &kRandomOption, sizeof( kRandomOption ) );
		m_vecCombinedRandomOptionExtension.push_back( kRandomOption );
	}
}

void	FLRandomOption::SerializeAllRandomOptionExtension( CAr & ar ) const
{
	SerializeRandomOptionExtension( ar );
	SerializeNewRandomOptionExtension( ar );
	SerializeCombinedRandomOptionExtension( ar );
}

void	FLRandomOption::DeserializeAllRandomOptionExtension( CAr & ar )
{
	DeserializeRandomOptionExtension( ar );
	DeserializeNewRandomOptionExtension( ar );
	DeserializeCombinedRandomOptionExtension( ar );
}
