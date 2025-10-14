
#include "stdafx.h"

#include "FLAdbillProtocol.h"
#include "../_Common/buyinginfo.h"


#pragma pack( 1 )

// 시스템 메시지를 넣기위해 사이즈를 아는 더미 해더
// 내부적으로만 사용하며 외부로 Send 할 경우 추가해서는 안된다.
struct	T_DUMMY_HEADER
{
	u_long	nSize;	// header 미포함
};

#pragma pack()

FLStreamProtocol*	FLAdbillProtocol::GetInstance()
{
	static FLAdbillProtocol xAdbillProtocol;
	return &xAdbillProtocol;
}

FLAdbillProtocol::FLAdbillProtocol()
{
}

FLAdbillProtocol::~FLAdbillProtocol()
{
}

bool	FLAdbillProtocol::FetchStream( CBuffer** pBuffer, CBufferQueue* pQueue )
{
	// 더미 해더를 추가하기 위해 받은 데이터를 새로운 버퍼로 복사하고
	// 기존 버퍼의 데이터를 앞으로 당긴다.

	u_long nRemnant;
	LPBYTE ptr	= (*pBuffer)->GetReadableBuffer( &nRemnant );
	CBuffer* pCompleted = NULL;

	while( 1 )
	{
		// 해더를 완전히 다 받지 못했거나 버퍼에 여유가 없다면
		if( nRemnant < sizeof( BUYING_INFO ) )
		{
			return true;
		}
		else	// 다 받았다.
		{
			if( pCompleted == NULL || pCompleted->GetWritableBufferSize() < (sizeof( T_DUMMY_HEADER ) + sizeof( BUYING_INFO )) )
			{
				pCompleted = CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
				if( pCompleted == NULL )
				{
					return false;
				}

				pQueue->AddTail( pCompleted );
			}

			// 더미 해더 복사
			T_DUMMY_HEADER tHeader;
			tHeader.nSize = sizeof( BUYING_INFO );

			/*
			T_C_BUFFER_INFO tBufferInfo[2];
			tBufferInfo[0].pBuffer = &tHeader;
			tBufferInfo[0].nSize = sizeof( tHeader );

			tBufferInfo[1].pBuffer = ptr;
			tBufferInfo[1].nSize = sizeof( BUYING_INFO );

			pQueue->WriteToOneBuffer( tBufferInfo, 2 );
			*/

			pCompleted->Write( &tHeader, sizeof( tHeader ) );
			pCompleted->Write( ptr, sizeof( BUYING_INFO ) );

			pCompleted->IncBlockCount();

			nRemnant	-= sizeof( BUYING_INFO );
			ptr			+= sizeof( BUYING_INFO );
		}
	}

	return true;
}

bool	FLAdbillProtocol::PushSystemStream( CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize )
{
	T_DUMMY_HEADER tHeader;
	tHeader.nSize = uDataSize;

	if( pBuffer->Write( &tHeader, sizeof( tHeader ) ) == false )	return false;
	if( pBuffer->Write( lpData, uDataSize ) == false )	return false;

	return true;
}

bool	FLAdbillProtocol::SendStream( CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize )
{
	if( pQueue->Write( lpData, uDataSize ) == false )
	{
		return false;
	}

	return true;
}

FLStreamProtocol*	FLAdbillProtocol::Clone()
{
	return this;
}

void	FLAdbillProtocol::Release()
{
}

void	FLAdbillProtocol::GetData( void* pStream, void** pData, u_long* nDataSize )
{
	T_DUMMY_HEADER* pHeader = reinterpret_cast< T_DUMMY_HEADER* >( pStream );
	*pData = pHeader+1;
	*nDataSize = pHeader->nSize;
}

void*	FLAdbillProtocol::GetNextStream( void* pStream )
{
	T_DUMMY_HEADER* pHeader = reinterpret_cast< T_DUMMY_HEADER* >( pStream );
	char* pStart = reinterpret_cast< char* >( pHeader );
	return pStart + sizeof( *pHeader ) + pHeader->nSize;
}

