#include "stdafx.h"
#include "accessory.h"

CAccessoryProperty::CAccessoryProperty()
{
	memset( m_adwProbability, 0, sizeof(m_adwProbability) );
}

CAccessoryProperty::~CAccessoryProperty()
{
	for( std::map<DWORD, std::vector<SINGLE_DST>*>::iterator i = m_mapAccessory.begin(); i != m_mapAccessory.end(); ++i )
	{
		std::vector<SINGLE_DST>* pVector	= i->second;
		SAFE_DELETE_ARRAY( pVector );
	}
	m_mapAccessory.clear();
}

CAccessoryProperty*	CAccessoryProperty::GetInstance( void )
{
	static	CAccessoryProperty	sAccessoryProperty;
	return &sAccessoryProperty;
}

BOOL CAccessoryProperty::IsAccessory( DWORD dwItemId )
{
	std::map<DWORD, std::vector<SINGLE_DST>*>::iterator i	= m_mapAccessory.find( dwItemId );
	return( i != m_mapAccessory.end() );
}

std::vector<SINGLE_DST>* CAccessoryProperty::GetDst( DWORD dwItemId, int nAbilityOption )
{
	std::map<DWORD, std::vector<SINGLE_DST>*>::iterator i	= m_mapAccessory.find( dwItemId );
	if( i != m_mapAccessory.end() )
	{
//		if( nAbilityOption >= 0 && static_cast<int>( i->second->size() ) > nAbilityOption )
		{
			return &( i->second[nAbilityOption] );
		}
	}

	return NULL;
}

BOOL CAccessoryProperty::LoadScript( LPCTSTR szFile )
{
	CScript s;
	if( s.Load( szFile ) == FALSE )
		return FALSE;

	s.GetToken();	// subject or FINISHED
	while( s.tok != FINISHED )
	{
		if( s.Token == _T( "Probability" ) )	// 아이템 각성
		{
			int i	= 0;
			s.GetToken();	// {
			DWORD	dwProbability	= s.GetNumber();
			while( *s.token != '}' )
			{
				m_adwProbability[i++]	= dwProbability;		// TODO : @@@@
				dwProbability	= s.GetNumber();
			}
		}
		else if( s.Token == _T( "Accessory" ) )
		{
			s.GetToken();	// {
			DWORD dwItemId	= s.GetNumber();
			while( *s.token != '}' )
			{
				s.GetToken();	// {
				std::vector<SINGLE_DST>* pVector	= new std::vector<SINGLE_DST>[_MAX_ENCHANT_LEVEL_ACCESSORY+1];
				int nAbilityOption	= s.GetNumber();
				while( *s.token != '}' )
				{
					FLASSERT( nAbilityOption >= 0 && nAbilityOption <= _MAX_ENCHANT_LEVEL_ACCESSORY );
					s.GetToken();	// {
					SINGLE_DST dst;
					dst.nDst	= s.GetNumber();
					while( *s.token != '}' )
					{
						dst.nAdj	= s.GetNumber();
						pVector[nAbilityOption].push_back( dst );
						dst.nDst	= s.GetNumber();
					}
					nAbilityOption	= s.GetNumber();
				}
				bool b	= m_mapAccessory.insert( std::map<DWORD, std::vector<SINGLE_DST>*>::value_type( dwItemId, pVector ) ).second;
				if( b == false )
				{
					SAFE_DELETE_ARRAY( pVector );
				}
				dwItemId	= s.GetNumber();
			}
		}
		s.GetToken();	//
	}
	return TRUE;
}