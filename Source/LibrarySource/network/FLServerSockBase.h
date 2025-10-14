
#ifndef	__FLSERVERSOCKBASE_H__
#define	__FLSERVERSOCKBASE_H__


#include <map>

#include "FLNetworkCommon.h"
#include "FLSock.h"
#include "FLAccepter.h"
#include "FLStreamProtocol.h"

#include "FLDOS.h"
#include "../thread/FLCriticalSection.h"
#include "../thread/FLAutoLock.h"


template< typename T >
class	FLServerSockBase : public CSock, public FLAcceptHandler
{
private:

	typedef	std::map< DPID, T* >			CHILD_LIST;
	typedef	typename CHILD_LIST::iterator	CHILD_ITER;

	FLAccepter			m_cAccepter;

	CMclCritSec			m_AddRemoveLock;
	CHILD_LIST			m_mapChild;

	FLStreamProtocol*	m_pProtocol;

	CDos2				m_cDOS;

public:

	FLServerSockBase( FLStreamProtocol* pProtocol )
	{
		m_pProtocol = pProtocol->Clone();
	}
	virtual ~FLServerSockBase()
	{
		Close();

		if( m_pProtocol != NULL )
		{
			m_pProtocol->Release();
			m_pProtocol = NULL;
		}
	}

	bool	SendProtocolSync( T* pChild )
	{
		if( pChild->StartProtocol() == false )
		{
			return false;
		}

		return true;
	}

	bool	Send( const char* lpData, u_long dwDataSize, DPID dpidTo )
	{
		if( dpidTo == DPID_ALLPLAYERS )
		{
			CMclAutoLock	Lock( m_AddRemoveLock );

			CHILD_ITER pos = m_mapChild.begin();
			CHILD_ITER end = m_mapChild.end();

			for( ; pos != end; ++pos )
			{
				CSock* pChild	= pos->second;
				if( pChild != NULL )
				{
					pChild->Send( lpData, dwDataSize, pChild->GetHandle() );
				}
			}

			return true;
		}
		else
		{
			CMclAutoLock	Lock( m_AddRemoveLock );
			CSock* pChild	= Get( dpidTo );
			if( pChild != NULL )
			{
				if( pChild->Send( lpData, dwDataSize, dpidTo ) == true )
				{
					return true;
				}
			}
		}

		return false;
	}

	void	Close()
	{
		m_cAccepter.Destroy();
		CLOSE_SOCKET( m_hSocket );
		CloseAllChild();
	}

	bool	Shutdown( SOCKET hSocket )
	{
		CMclAutoLock	Lock( m_AddRemoveLock );
		CSock* pChild	= Get( hSocket );
		if( pChild )
		{
			closesocket( pChild->GetHandle() );
			return true;
		}

		return false;
	}
	void	ShutdownAddr( DWORD dwAddr )
	{
		CMclAutoLock	Lock( m_AddRemoveLock );

		CHILD_ITER pos = m_mapChild.begin();
		CHILD_ITER end = m_mapChild.end();

		for( ; pos != end; ++pos )
		{
			CSock* pChild	= pos->second;
			if( pChild->GetPeerAddr( pChild->GetID() ) == dwAddr )
			{
				shutdown( pChild->GetHandle(), SD_BOTH );
			}
		}
	}
	bool	CloseConnection( SOCKET hSocket )
	{
		return CloseChild( hSocket );
	}

	bool	GetPeerAddr( DPID dpid, char* lpAddr, size_t cchAddr )
	{
		FLASSERT( lpAddr );

		CMclAutoLock	Lock( m_AddRemoveLock );
		CSock* pChild	= Get( dpid );
		if( pChild != NULL )
		{
			return pChild->GetPeerAddr( dpid, lpAddr, cchAddr );
		}

		if( lpAddr != NULL && cchAddr > 0 )
		{
			*lpAddr = '\0';
		}

		return false;
	}
	DWORD	GetPeerAddr( DPID dpid )
	{
		CMclAutoLock	Lock( m_AddRemoveLock );
		CSock* pChild = Get( dpid );
		if( pChild != NULL )
		{
			return pChild->GetPeerAddr( dpid );
		}

		return 0;
	}

	DWORD	GetDebugInfo( SOCKET hSocket )
	{
		CMclAutoLock	Lock( m_AddRemoveLock );
		CSock* pChild	= Get( hSocket );
		if( pChild != NULL )
		{
			return pChild->GetDebugInfo( hSocket );
		}

		return 0;
	}

	CSock*	Get( SOCKET hSocket )
	{
		CHILD_ITER i = m_mapChild.find( hSocket );
		if( i != m_mapChild.end() )
		{
			return i->second;
		}

		return NULL;
	}

protected:

	bool	Accept( const char* lpAddr, WORD wPort )
	{
		if( m_cAccepter.Create( this, lpAddr, wPort ) == false )
		{
			return false;
		}

		if( m_cAccepter.Start( this ) == false )
		{
			m_cAccepter.Destroy();
			CLOSE_SOCKET( m_hSocket );
			return false;
		}

		return true;
	}

	bool	CheckDOS( SOCKET hSocket, DWORD dwAddr )
	{
		int i = m_cDOS.IsOver( dwAddr );
		if( i < 0 )
		{	// close
			ShutdownAddr( dwAddr );
			return false;
		}
		else if( i > 0 )
		{	// close this socket
			CLOSE_SOCKET( hSocket );
			return false;
		}

		return true;
	}

	T*		AddChild( SOCKET hSocket )
	{
		T* pChild = new T( m_pProtocol );
		pChild->Attach( hSocket );

		pChild->SetID( DPID_SERVERPLAYER );
		pChild->SetPeerID( hSocket );

		CMclAutoLock Lock( m_AddRemoveLock );

		CHILD_ITER i	= m_mapChild.find( hSocket );
		if( i != m_mapChild.end() )
		{
			CSock* ptr	= i->second;
			ptr->Detach();

			m_mapChild.erase( i );
			SAFE_DELETE( ptr );
		}

		m_mapChild.insert( CHILD_LIST::value_type( hSocket, pChild ) );
		return pChild;
	}

	bool	CloseChild( SOCKET hSocket )
	{
		CMclAutoLock	Lock( m_AddRemoveLock );

		CHILD_ITER i	= m_mapChild.find( hSocket );
		if( i != m_mapChild.end() )
		{
			CSock* pChild	= i->second;
			m_mapChild.erase( i );

			pChild->Close();
			SAFE_DELETE( pChild );
			return true;
		}

		return false;
	}

	void	CloseAllChild()
	{
		CMclAutoLock	Lock( m_AddRemoveLock );

		CHILD_ITER pos = m_mapChild.begin();
		CHILD_ITER end = m_mapChild.end();

		for( ; pos != end; ++pos )
		{
			CSock* pChild	= pos->second;

			pChild->Close();
			SAFE_DELETE( pChild );
		}

		m_mapChild.clear();
	}

};









#endif