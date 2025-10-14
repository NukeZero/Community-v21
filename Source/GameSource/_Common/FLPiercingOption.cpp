
#include "StdAfx.h"
#include "FLPiercingOption.h"


FLPiercingOption::FLPiercingOption()
{
	Clear();
}

FLPiercingOption::FLPiercingOption( const FLPiercingOption & rhs )
{
	Clear();
	*this	= rhs;
}

FLPiercingOption::~FLPiercingOption()
{
	Clear();
}

FLPiercingOption&	FLPiercingOption::operator = ( const FLPiercingOption & kPiercing )
{
	m_vecGeneralPiercing.assign( kPiercing.m_vecGeneralPiercing.begin(), kPiercing.m_vecGeneralPiercing.end() );
	m_vecUltimatePiercing.assign( kPiercing.m_vecUltimatePiercing.begin(), kPiercing.m_vecUltimatePiercing.end() );
	m_vecVisPetPiercing.assign( kPiercing.m_vecVisPetPiercing.begin(), kPiercing.m_vecVisPetPiercing.end() );
	m_vecCombinedGeneralPiercing.assign( kPiercing.m_vecCombinedGeneralPiercing.begin(), kPiercing.m_vecCombinedGeneralPiercing.end() );

	return *this;
}

void	FLPiercingOption::Clear()
{
	m_vecGeneralPiercing.clear();
	m_vecUltimatePiercing.clear();
	m_vecVisPetPiercing.clear();
	m_vecCombinedGeneralPiercing.clear();
}

void	FLPiercingOption::InitializeCombinedPiercingItem()
{
	m_vecCombinedGeneralPiercing.clear();
}

bool	FLPiercingOption::IsSetGeneralPiercingOption()
{
	if( m_vecGeneralPiercing.empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLPiercingOption::IsSetUltimatePiercingOption()
{
	if( m_vecUltimatePiercing.empty() == false )
	{
		return true;
	}

	return false;
}

bool	FLPiercingOption::IsExistGeneralPiercedItemID( const DWORD dwItemID )
{
	if( m_vecGeneralPiercing.empty() == true )
	{
		return false;
	}

	for( size_t Nth = 0; Nth < m_vecGeneralPiercing.size(); ++Nth )
	{
		if( m_vecGeneralPiercing.at( Nth ) == dwItemID )
		{
			return true;
		}
	}

	return false;
}

bool	FLPiercingOption::IsSetCombinedGeneralPiercingOption()
{
	if( m_vecCombinedGeneralPiercing.empty() == false )
	{
		return true;
	}

	return false;
}

const size_t	FLPiercingOption::GetGeneralPiercingSize() const
{
	return m_vecGeneralPiercing.size();
}

const size_t	FLPiercingOption::GetUltimatePiercingSize() const
{
	return m_vecUltimatePiercing.size();
}

const size_t	FLPiercingOption::GetVisPetPiercingSize() const
{
	return m_vecVisPetPiercing.size();
}

const size_t	FLPiercingOption::GetCombinedGeneralPiercingSize() const
{
	return m_vecCombinedGeneralPiercing.size();
}

const DWORD	FLPiercingOption::GetGeneralPiercingItemID( const size_t Nth ) const
{
	if( m_vecGeneralPiercing.empty() == false && m_vecGeneralPiercing.size() > Nth )
	{
		return m_vecGeneralPiercing.at( Nth );
	}

	return 0;
}

const DWORD	FLPiercingOption::GetUltimatePiercingItemID( const size_t Nth ) const
{
	if( m_vecUltimatePiercing.empty() == false && m_vecUltimatePiercing.size() > Nth )
	{
		return m_vecUltimatePiercing.at( Nth );
	}

	return 0;
}

const time_t	FLPiercingOption::GetVisPetPiercingItemKeepTime( const size_t Nth ) const
{
	if( m_vecGeneralPiercing.empty() == false && m_vecVisPetPiercing.empty() == false
		&& m_vecGeneralPiercing.size() > Nth && m_vecVisPetPiercing.size() > Nth )
	{
		return m_vecVisPetPiercing.at( Nth );
	}

	return 0;
}

const DWORD	FLPiercingOption::GetCombinedGeneralPiercingItemID( const size_t Nth ) const
{
	if( m_vecCombinedGeneralPiercing.empty() == false && m_vecCombinedGeneralPiercing.size() > Nth )
	{
		return m_vecCombinedGeneralPiercing.at( Nth );
	}

	return 0;
}

void	FLPiercingOption::SetGeneralPiercingSize( const size_t nSize )
{
	m_vecGeneralPiercing.resize( min( nSize, MAX_PIERCING ), 0 );
}

void	FLPiercingOption::SetUltimatePiercingSize( const size_t nSize )
{
	m_vecUltimatePiercing.resize( min( nSize, MAX_PIERCING_ULTIMATE ), 0 );
}

void	FLPiercingOption::SetVisPetPiercingSize( const size_t nSize )
{
	m_vecVisPetPiercing.resize( min( nSize, MAX_VIS ), 0 );
}

void	FLPiercingOption::SetCombinedGeneralPiercingSize( const size_t nSize )
{
	m_vecCombinedGeneralPiercing.resize( min( nSize, MAX_PIERCING ), 0 );
}

void	FLPiercingOption::SetGeneralPiercingItemID( const size_t Nth, const DWORD dwItemID )
{
	if( m_vecGeneralPiercing.size() > Nth )
	{
		m_vecGeneralPiercing.at( Nth )	= dwItemID;
	}
}

void	FLPiercingOption::SetUltimatePiercingItemID( const size_t Nth, const DWORD dwItemID )
{
	if( m_vecUltimatePiercing.size() > Nth )
	{
		m_vecUltimatePiercing.at( Nth )	= dwItemID;
	}
}

void	FLPiercingOption::SetVisPetPiercingItemKeepTime( const size_t Nth, const time_t tKeepTime )
{
	if( m_vecGeneralPiercing.size() > Nth && m_vecVisPetPiercing.size() > Nth )
	{
		m_vecVisPetPiercing.at( Nth )	= tKeepTime;
	}
}

void	FLPiercingOption::SetCombinedGeneralPiercingItemID( const size_t Nth, const DWORD dwItemID )
{
	if( m_vecCombinedGeneralPiercing.size() > Nth )
	{
		m_vecCombinedGeneralPiercing.at( Nth )	= dwItemID;
	}
}

void	FLPiercingOption::SwapVisItem( const size_t nSrc, const size_t nDest )
{
	const size_t nSize			= m_vecGeneralPiercing.size();
	const size_t nKeepTimeSize	= m_vecVisPetPiercing.size();
	if( nSrc >= nSize || nDest >= nSize || nSrc >= nKeepTimeSize || nDest >= nKeepTimeSize )
	{
		return;
	}

	const DWORD dwSrcItemID	= GetGeneralPiercingItemID( nSrc );
	const time_t tSrcTime	= GetVisPetPiercingItemKeepTime( nSrc );

	SetGeneralPiercingItemID( nSrc, GetGeneralPiercingItemID( nDest ) );
	SetVisPetPiercingItemKeepTime( nSrc, GetVisPetPiercingItemKeepTime( nDest ) );
	SetGeneralPiercingItemID( nDest, dwSrcItemID );
	SetVisPetPiercingItemKeepTime( nDest, tSrcTime );
}

void	FLPiercingOption::SerializePiercingOption( CAr & ar ) const
{
	size_t nSize = GetGeneralPiercingSize();

	ar << nSize;
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		ar << GetGeneralPiercingItemID( Nth );
	}

	nSize = GetUltimatePiercingSize();
	ar << nSize;
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		ar << GetUltimatePiercingItemID( Nth );
	}

	nSize = GetVisPetPiercingSize();
	ar << nSize;
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		ar << ( GetVisPetPiercingItemKeepTime( Nth ) - time_null() );
	}
}

void	FLPiercingOption::SerializeCombinedPiercingOption( CAr & ar ) const
{
	size_t nSize = GetCombinedGeneralPiercingSize();
	ar << nSize;
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		ar << GetCombinedGeneralPiercingItemID( Nth );
	}
}

void	FLPiercingOption::DeserializePiercingOption( CAr & ar )
{
	size_t nSize = 0;

	m_vecGeneralPiercing.clear();
	ar >> nSize;
	SetGeneralPiercingSize( nSize );
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		DWORD dwItemID = 0;
		ar >> dwItemID;
		SetGeneralPiercingItemID( Nth, dwItemID );
	}

	m_vecUltimatePiercing.clear();
	ar >> nSize;
	SetUltimatePiercingSize( nSize );
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		DWORD dwItemID = 0;
		ar >> dwItemID;
		SetUltimatePiercingItemID( Nth, dwItemID );
	}

	m_vecVisPetPiercing.clear();
	ar >> nSize;
	SetVisPetPiercingSize( nSize );
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		time_t tTime = 0;
		ar >> tTime;
		SetVisPetPiercingItemKeepTime( Nth, tTime + time_null() );
	}
}

void	FLPiercingOption::DeserializeCombinedPiercingOption( CAr & ar )
{
	size_t nSize = 0;

	m_vecCombinedGeneralPiercing.clear();
	ar >> nSize;
	SetCombinedGeneralPiercingSize( nSize );
	for( size_t Nth = 0; Nth < nSize; ++Nth )
	{
		DWORD dwItemID = 0;
		ar >> dwItemID;
		SetCombinedGeneralPiercingItemID( Nth, dwItemID );
	}
}

void	FLPiercingOption::SerializeAllPiercingOption( CAr & ar ) const
{
	SerializePiercingOption( ar );
	SerializeCombinedPiercingOption( ar );
}

void	FLPiercingOption::DeserializeAllPiercingOption( CAr & ar )
{
	DeserializePiercingOption( ar );
	DeserializeCombinedPiercingOption( ar );
}
