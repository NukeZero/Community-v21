#pragma once

#ifdef _xtrap
#include <xtrap/Server/20101129_525_Xtrap_S_Interface.h>

class CDPMng;


#define				CSFILE_NUM					1

class FLXTrap_Server
{
public:
	enum 
	{
		XTRAP_DEFAULT_TIMEOUT		= XTRAP_CS4_MAXTIMEOUT,
		
		BW_XTRAP_SYSTEM_ERROR		= 0x10000000,
		BW_XTRAP_WISELOGIC_ERROR	= 0x20000000,
		BW_XTRAP_NOTLOADED_DLL		= 0x40000000,
		BW_XTRAP_WISELOGIC_SUPPORT	= 0x80000000,

		XTRAP_CHECK_EVENT_INTERVAL_MS	= 20000 - 100,		
		//START_TIME_OUT_LIMIT	= 600,
		XTRAP_UPDATE_INTERVAL		= 800, 
	};

	struct FLSession
	{
		FLSession()	: dwLastUpdateTick( 0 )	{}

		char				buffer[ XTRAP_CS4_BUFSIZE_SESSION ];
		DWORD				dwLastUpdateTick;
	};

public:
	FLXTrap_Server();
	virtual ~FLXTrap_Server();

	static FLXTrap_Server &	GetInstance();

	BOOL					Start();

	void					SetActive( const BOOL bActive );
	BOOL					IsActive() const;

	BOOL					IsUpdateTime( const DWORD dwCurrTick );

//-------------------------------------------------------------------------------------------------------//

	BOOL					AllocSessionBuffer( /*CDPMng & DPMng, const DPID dpid,*/ FLSession & o_Session );
	void					UpdateCheckSession( CDPMng & DPMng, const DPID dpid, FLSession & o_Session/*, char * o_pPacketBuffer*/ );
	void					Response( FLSession & o_Session, const char * pRecvData );
	BOOL					SetActiveCode( const unsigned int ulActiveCode );
	BOOL					SetAllowDelayCycle( const unsigned int uAllowDelayCycle );

private:
	BOOL					loadMapFile( const char * szMapFile );
	BOOL					start();

public:
	volatile BOOL			m_bActive;

	DWORD					m_dwLastUpdate;

	size_t					m_nMapLoadedCount;

#if CSFILE_NUM > 0 
	unsigned char			m_Map[ CSFILE_NUM ][ XTRAP_CS4_BUFSIZE_MAP ];
#endif
};

// SessionBuffer, ÆÐÅ¶ 



#endif



