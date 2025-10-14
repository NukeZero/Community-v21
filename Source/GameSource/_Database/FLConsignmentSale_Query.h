#pragma once

#include "FLQueryCommand.h"

namespace nsConsignmentSale {


enum E_QUERY_KEY
{
	QUERY_KEY_SYNC = 0,
	//QUERY_KEY_ASYNC,
};





class FLQuery_Regist : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_Regist( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}

	virtual bool 			Execute( CQuery & query );
};


class FLQuery_CancelRegisted : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_CancelRegisted( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}

	virtual bool 			Execute( CQuery & query );
};

class FLQuery_BuyItem : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_BuyItem( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}

	virtual bool 			Execute( CQuery & query );
};

class FLQuery_CollectSaleGold : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_CollectSaleGold( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}

	virtual bool 			Execute( CQuery & query );
};


class FLQuery_SaleGoldInfoList : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_SaleGoldInfoList( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}

	virtual bool 			Execute( CQuery & query );
};


class FLQuery_Load : public FLQueryCommand
{
public:
	FLQuery_Load() {}

	virtual bool 			Execute( CQuery & query );
};


class FLQuery_Search : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_Search( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}

	virtual bool 			Execute( CQuery & query );
};

} /* FLQueryCommand */