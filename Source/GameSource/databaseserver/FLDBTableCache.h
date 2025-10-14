#pragma once

//#include "Query.h"


class FLDBTableInfo
{
//private:
//	class FLDBTableData
//	{
//		//총사이즈를 얻고 offset을 만든다.
//		BYTE *					m_pData;
//		size_t					m_uDataSize;
//	};

public:
	FLDBTableInfo()					{}
	virtual ~FLDBTableInfo()		{}

	const CQuery::FLColumnInfo &	GetColumnInfo( const char * szColumn );
	const CQuery::FLColumnInfo &	GetColumnInfo( const int nColumnIndex );
	
private:
	BOOL							loadColumnInfo();
	BOOL							loadData();

	// Send TableInfo( )
	// MakeSelectQuery( 패킷 )
    
private:
	typedef							std::map< std::string, CQuery::FLColumnInfo >	ColumnInfoMap;

//	ColumnInfoMap					m_mapColumnInfo;
	CQuery::ColumnInfoVec			m_vecColumnInfo;
//	FLDBTableData					m_TableData;
};