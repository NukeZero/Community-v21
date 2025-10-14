#pragma once

#include "FLQueryCommand.h"

class FLItemGold_LogQuery : public FLQueryCommand
{
public:
	FLItemGold_LogQuery( const LogItemInfo & info );

	virtual bool 			Execute( CQuery & query );

private:
	const LogItemInfo 		m_kInfo;
};


const size_t				LOG_QUERY_STATE_BUFFER_LEN	= 3;

class FLItem_LogQuery : public FLQueryCommand
{
public:
	FLItem_LogQuery( const char * szQueryState, const LogItemInfo & info, const u_long uPlayerID, const int nNo, const char * szItemName );

	virtual bool			Execute( CQuery & query );

private:
	char					m_szQueryState[ LOG_QUERY_STATE_BUFFER_LEN ];
	const LogItemInfo		m_kItemInfo;
	const u_long			m_uPlayerID;
	const int				m_nNo;
	char					m_szItemName[ ITEM_NAME_BUFFER_LEN ];
};