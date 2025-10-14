#pragma once

#include "FLQueryCommand.h"


class FLQuery_SavePlayer : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_SavePlayer::FLQuery_SavePlayer( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize
		, CMover * pMover );
			//: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize )/*, m_pMover( pMover )*/ { m_kMover.Copy( pMover, TRUE );}

	virtual bool			Execute( CQuery & query );

private:
	bool					execQuery( CQuery & query );

public:
	CMover 					m_kMover;
};


class FLQuery_LoadPlayer : public FLQueryCommand_HasOverlappedPlus
{
public:
	FLQuery_LoadPlayer::FLQuery_LoadPlayer( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}

	virtual bool			Execute( CQuery & query );
};

		//case SENDPLAYERLIST:
		//	SendPlayerList( pQuery, lpDbOverlappedPlus );
		//	break;
		//case JOIN:
		//	Join( pQuery, pQuery1, pQueryLog, lpDbOverlappedPlus );
		//	break;
		//case CREATEPLAYER:
		//	CreatePlayer( pQuery, lpDbOverlappedPlus );
		//	break;
		//case REMOVEPLAYER:
		//	RemovePlayer( pQuery, lpDbOverlappedPlus );
		//	break;
		//case QM_LOGIN_PROTECT:
		//	LoginProtectCert( pQuery, lpDbOverlappedPlus );
		//	break;

//
//class FLQuery_DisableItems : public FLQueryCommand_HasOverlappedPlus
//{
//public:
//	FLQuery_DisableItems::FLQuery_DisableItems( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
//		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}
//
//	virtual void			Execute( CQuery & query );
//};
//
//
//class FLQuery_DeleteDisableItems : public FLQueryCommand_HasOverlappedPlus
//{
//public:
//	FLQuery_DeleteDisableItems::FLQuery_DeleteDisableItems( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
//		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}
//
//	virtual void			Execute( CQuery & query );
//};
//
//
//class FLQuery_InsertItems : public FLQueryCommand_HasOverlappedPlus
//{
//public:
//	FLQuery_InsertItems::FLQuery_InsertItems( const DPID dpid, const DPID dpidCache, const DPID dpidUser, LPBYTE lpBuf, u_long uBufSize ) 
//		: FLQueryCommand_HasOverlappedPlus( dpid, dpidCache, dpidUser, lpBuf, uBufSize ) {}
//
//	virtual void			Execute( CQuery & query );
//};