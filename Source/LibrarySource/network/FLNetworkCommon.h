
#ifndef	__FLNETWORKCOMMON_H__
#define	__FLNETWORKCOMMON_H__



#include "../common/FLCommon.h"
#include "../thread/FLThreadCommon.h"

#include "FLNetworkLogAdapter.h"

#include <winsock2.h>


#define CLOSE_SOCKET( s )			\
	{								\
		CSock::CloseSocket( (s) );	\
		(s) = INVALID_SOCKET;		\
	}


#define	CLOSE_WSAEVENT(h)	\
	{	\
		if( h != WSA_INVALID_EVENT )	\
		{	\
			WSACloseEvent( h );	\
			( h )	= WSA_INVALID_EVENT;	\
		}	\
	}


//////////////////////////////////////////////////////////////////////////
// from dplay.h
/*
 * DPID
 * DirectPlay player and group ID
 */
typedef	SOCKET	DPID, FAR *LPDPID;

/*
 * DPID that system messages come from
 */
#define DPID_SYSMSG         0

/*
 * DPID representing all players in the session
 */
#define DPID_ALLPLAYERS     0

/*
 * DPID representing the server player
 */
#define DPID_SERVERPLAYER	1

/*
 * The player ID is unknown (used with e.g. DPSESSION_NOMESSAGEID)
 */
#define DPID_UNKNOWN		0xFFFFFFFF


/*
 * Used in the dwPlayerType field to indicate if it applies to a group
 * or a player
 */
#define DPPLAYERTYPE_GROUP          0x00000000
#define DPPLAYERTYPE_PLAYER         0x00000001

/*
 * A new player or group has been created in the session
 * Use DPMSG_CREATEPLAYERORGROUP.  Check dwPlayerType to see if it
 * is a player or a group.
 */
#define DPSYS_CREATEPLAYERORGROUP   0x0003  

/*
 * A player has been deleted from the session
 * Use DPMSG_DESTROYPLAYERORGROUP
 */
#define DPSYS_DESTROYPLAYERORGROUP  0x0005  

 
/*
 * DPNAME
 * Used to hold the name of a DirectPlay entity
 * like a player or a group
 */
typedef struct 
{
    DWORD   dwSize;             // Size of structure
    DWORD   dwFlags;            // Not used. Must be zero.
    union
    {                           // The short or friendly name
        LPWSTR  lpszShortName;  // Unicode
        LPSTR   lpszShortNameA; // ANSI
    };
    union
    {                           // The long or formal name
        LPWSTR  lpszLongName;   // Unicode
        LPSTR   lpszLongNameA;  // ANSI
    };

} DPNAME, FAR *LPDPNAME;

/*
 * LPCDPNAME
 * A constant pointer to DPNAME
 */
typedef const DPNAME FAR *LPCDPNAME;

/*
 * DPMSG_GENERIC
 * Generic message structure used to identify the message type.
 */
typedef struct
{
    DWORD       dwType;         // Message type
} DPMSG_GENERIC, FAR *LPDPMSG_GENERIC;

/*
 * DPMSG_CREATEPLAYERORGROUP
 * System message generated when a new player or group
 * created in the session with information about it.
 */
typedef struct
{
    DWORD       dwType;         // Message type
    DWORD       dwPlayerType;   // Is it a player or group
    DPID        dpId;           // ID of the player or group
    DWORD       dwCurrentPlayers;   // current # players & groups in session
    LPVOID      lpData;         // pointer to remote data
    DWORD       dwDataSize;     // size of remote data
    DPNAME      dpnName;        // structure with name info
	// the following fields are only available when using
	// the IDirectPlay3 interface or greater
    DPID	    dpIdParent;     // id of parent group
	DWORD		dwFlags;		// player or group flags
} DPMSG_CREATEPLAYERORGROUP, FAR *LPDPMSG_CREATEPLAYERORGROUP;

/*
 * DPMSG_DESTROYPLAYERORGROUP
 * System message generated when a player or group is being
 * destroyed in the session with information about it.
 */
typedef struct
{
    DWORD       dwType;         // Message type
    DWORD       dwPlayerType;   // Is it a player or group
    DPID        dpId;           // player ID being deleted
    LPVOID      lpLocalData;    // copy of players local data
    DWORD       dwLocalDataSize; // sizeof local data
    LPVOID      lpRemoteData;   // copy of players remote data
    DWORD       dwRemoteDataSize; // sizeof remote data
	// the following fields are only available when using
	// the IDirectPlay3 interface or greater
    DPNAME      dpnName;        // structure with name info
    DPID	    dpIdParent;     // id of parent group	
	DWORD		dwFlags;		// player or group flags
} DPMSG_DESTROYPLAYERORGROUP, FAR *LPDPMSG_DESTROYPLAYERORGROUP;

//////////////////////////////////////////////////////////////////////////


#endif