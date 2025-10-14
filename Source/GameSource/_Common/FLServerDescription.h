#ifndef __FLSERVERDESCRIPTION_H__
#define __FLSERVERDESCRIPTION_H__


//////////////////////////////////////////////////////////////////////////
typedef std::vector< T_SERVER_DESC >		ServerDescVec;
typedef ServerDescVec::iterator				ServerDescItr;
typedef ServerDescVec::const_iterator		ServerDescCItr;
//////////////////////////////////////////////////////////////////////////

class FLServerDescription
{
	public:
		FLServerDescription();
		~FLServerDescription();

		static FLServerDescription*		GetInstance();

		const T_SERVER_DESC*			FindByID( DWORD dwParentID, DWORD dwMyID ) const;
		const T_SERVER_DESC*			FindByNth( DWORD dwParentID, DWORD dwNth ) const;
		

		void			Add( const T_SERVER_DESC* pServerDesc );
		void			Clear();
		
		long			IncreaseConnectionCount( DWORD dwParentID, DWORD dwMyID );
		long			DecreaseConnectionCount( DWORD dwParentID, DWORD dwMyID );
		void			SetConnectionCount( DWORD dwParentID, DWORD dwMyID, long lConnectionCount );
		void			SetConnectionMode( DWORD dwParentID, DWORD dwMyID, long lCanConnection );

		void			GetAllServerDescription( ServerDescVec* pServerDesc ) const;
		void			GetCanConnectionServerDescription( ServerDescVec* pServerDesc ) const;


		void			SetServerIndex( DWORD dwServerIndex );
		DWORD			GetServerIndex();


	private:
		T_SERVER_DESC*	Find( DWORD dwParentID, DWORD dwMyID );

	
	private:
		DWORD			m_dwServerIndex;
		ServerDescVec	m_vecServerDesc;
};

#define		g_pServerDescription		FLServerDescription::GetInstance()

#endif // __FLSERVERDESCRIPTION_H__
