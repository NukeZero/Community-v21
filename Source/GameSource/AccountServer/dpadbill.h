#ifndef __DPADBILL_H__
#define	__DPADBILL_H__

#include <network/FLDPMng.h>

#include "FLASDBManager.h"



class CDPAdbill : public CDPMng
{
	private:
		DWORD	m_dwKey;
	public:
	//	Constructions
		CDPAdbill();
		virtual	~CDPAdbill();
	//	Overrides
		virtual void	SysMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );
		virtual void	UserMessageHandler( LPDPMSG_GENERIC lpMsg, DWORD dwMsgSize, DPID idFrom );

		static	CDPAdbill*	GetInstance();
};

//________________________________________________________________________________

class CBuyingInfoManager
{
	public:
		static	CBuyingInfoManager *	GetInstance();

	private:
		std::map<DWORD, PBUYING_INFO3>	m_mapPbi3;

	public:
		CMclCritSec		m_AddRemoveLock;

	public:
	//	Constructions
		CBuyingInfoManager()	{}
		~CBuyingInfoManager()	{	Clear();	}
		void	Clear()
			{
				m_AddRemoveLock.Enter();

				for( std::map<DWORD, PBUYING_INFO3>::iterator i = m_mapPbi3.begin(); i != m_mapPbi3.end(); ++i )
				{
					SAFE_DELETE( i->second );
				}

				m_mapPbi3.clear();

				m_AddRemoveLock.Leave();
			}

	//	Operations
		void	Add( PBUYING_INFO3 pbi3 )
			{
				m_AddRemoveLock.Enter();
				m_mapPbi3.insert( std::map<DWORD, PBUYING_INFO3>::value_type( pbi3->dwKey, pbi3 ) );
				m_AddRemoveLock.Leave();

				return;
			}

		void	Remove( DWORD dwKey )
			{
				m_AddRemoveLock.Enter();
				m_mapPbi3.erase( dwKey );
				m_AddRemoveLock.Leave();

				return;
			}

		PBUYING_INFO3	Get( DWORD dwKey )
			{
				std::map<DWORD, PBUYING_INFO3>::iterator i	= m_mapPbi3.find( dwKey );

				if( i != m_mapPbi3.end() )
				{
					return i->second;
				}

				return NULL;
			}
		
		void	Process()
			{
				DWORD dwTickCount	= GetTickCount();
				std::vector<DWORD>	adwKey;
				
				m_AddRemoveLock.Enter();
				for( std::map<DWORD, PBUYING_INFO3>::iterator i = m_mapPbi3.begin(); i != m_mapPbi3.end(); ++i )
				{
					PBUYING_INFO3 pbi3	= i->second;
					if( dwTickCount - pbi3->dwTickCount > SEC( 3 ) )
					{
						CDPAdbill::GetInstance()->Send( pbi3, sizeof(BUYING_INFO), pbi3->dpid );
						adwKey.push_back( pbi3->dwKey );
					}
				}
				
	//			Remove
				for( DWORD j = 0; j < adwKey.size(); j++ )
				{
					PBUYING_INFO3 ptr	= CBuyingInfoManager::GetInstance()->Get( adwKey.at( j ) );
					if( ptr )
					{
						FLTRACE_LOG( PROGRAM_NAME, _T( "ACCOUNTSERVER.EXE\t// timeout\t// dwPlayerId = %07d\t// dwItemId = %d" ), ptr->dwPlayerId, ptr->dwItemId );
						/*
						ptr->szBxaid		// 구매 번호
						ptr->dwItemId	// 아이템 인덱스
						ptr->dwItemNum	// 아이템 개수
						ptr->dwServerIndex	// 서버 인덱스
						ptr->dwPlayerId		// 플레이어 식별자
						ptr->dwRetVal	// 지급 성공 여부, 성공 : 1, 실패 : 0
						*/
						BUYING_INFO2	bi2;
						DWORD sn = 0;
						FLStrcpy( bi2.szBxaid, _countof( bi2.szBxaid ), ptr->szBxaid );
						bi2.dwItemId = ptr->dwItemId;
						bi2.dwItemNum = ptr->dwItemNum;
						bi2.dwServerIndex = ptr->dwServerIndex;
						bi2.dwRetVal = ptr->dwRetVal;
						bi2.dwPlayerId = ptr->dwPlayerId;
						CAr ar;
						ar.Write( (void*)&bi2, sizeof(BUYING_INFO2) );
						ar << sn;

						u_long nBufSize;
						LPBYTE lpData	= ar.GetBuffer( &nBufSize );

						if( nBufSize < 0 )
						{
							FLERROR_LOG( PROGRAM_NAME, _T( "nBufSize < 0" ) );
						}

						PT_AS_DBOVERLAPPED lpDbOverlappedPlus		= g_xASDBManager->m_pDbIOData->Alloc();
						memcpy( lpDbOverlappedPlus->lpBuf, lpData, nBufSize );
						lpDbOverlappedPlus->dwBufSize	= (DWORD)( nBufSize );
						lpDbOverlappedPlus->nQueryMode	= LOG_SM_ITEM;
						PostQueuedCompletionStatus( g_xASDBManager->m_hDbCompletionPort, 1, NULL, &lpDbOverlappedPlus->Overlapped );
					}
					Remove( adwKey.at( j ) );
					SAFE_DELETE( ptr );
				}

				m_AddRemoveLock.Leave();

				adwKey.clear();
			}
};

#define g_xBuyingInfoManager	CBuyingInfoManager::GetInstance()

#endif	// __DPADBILL_H__