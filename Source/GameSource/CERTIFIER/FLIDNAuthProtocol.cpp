
#include "stdafx.h"

#include "FLIDNAuthDefine.h"
#include "FLIDNAuthProtocol.h"



FLStreamProtocol*	FLIDNAuthProtocol::GetInstance()
{
	static FLIDNAuthProtocol xIDNAuthProtocol;
	return &xIDNAuthProtocol;
}

FLIDNAuthProtocol::FLIDNAuthProtocol()
{
}

FLIDNAuthProtocol::~FLIDNAuthProtocol()
{
}

bool	FLIDNAuthProtocol::FetchStream( CBuffer** pBuffer, CBufferQueue* pQueue )
{
	u_long nRemnant;
	LPBYTE ptr	= (*pBuffer)->GetReadableBuffer( &nRemnant );

//	CBuffer* pOld	= NULL;
	u_long uPacketSize;

	while( 1 )
	{
		if( nRemnant < sizeof( T_IDN_AUTH_HEADER ) )	// 해더를 완전히 다 받지 못했다.
		{
			if( (*pBuffer)->GetBlockCount() > 0 )	// 다 받은 메시지가 있다면
			{
				CBuffer* pOld	= (*pBuffer);

				(*pBuffer)	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
				if( (*pBuffer) == NULL )
				{
					SAFE_DELETE( *pBuffer );
					*pBuffer = pOld;
					return false;
				}

				(*pBuffer)->Write( ptr, nRemnant );
				pOld->CutTail( nRemnant );

				pQueue->AddTail( pOld );
			}

			return true;
		}
		else	// 해더 이상 받았다.
		{
			T_IDN_AUTH_HEADER* pHeader = reinterpret_cast< T_IDN_AUTH_HEADER* >( ptr );
			if( pHeader->byMark != IDN_AUTH_FIXED_MARK )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "IDN Auth Protocol header mark failed. RecvMark: %x, SrcMark: %x" ), pHeader->byMark, IDN_AUTH_FIXED_MARK );
				return false;
			}

			uPacketSize	= pHeader->GetSize();

			if( nRemnant < uPacketSize )	// 데이터를 완전히 다 받지 않았다.
			{
				if( uPacketSize > (*pBuffer)->GetSize() || (*pBuffer)->GetBlockCount() > 0 )	// 버퍼가 부족하거나 받은 데이터가 있다면
				{
					CBuffer* pOld	= (*pBuffer);

					(*pBuffer)	= CBufferFactory::GetInstance().CreateBuffer( uPacketSize );
					if( (*pBuffer) == NULL )
					{
						SAFE_DELETE( *pBuffer );
						*pBuffer = pOld;
						return false;
					}

					(*pBuffer)->Write( ptr, nRemnant );
					pOld->CutTail( nRemnant );

					if( pOld->GetBlockCount() > 0 )
					{
						pQueue->AddTail( pOld );
					}
					else
					{
						SAFE_DELETE( pOld );
					}
				}

				return true;
			}
			else	// completion
			{
				(*pBuffer)->IncBlockCount();
				nRemnant	-= ( uPacketSize );
				ptr			+= ( uPacketSize );
			}
		}
	}

	return true;
}

bool	FLIDNAuthProtocol::PushSystemStream( CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize )
{
	T_IDN_AUTH_HEADER tHeader;
	tHeader.SetSize( static_cast< WORD >( uDataSize ) + sizeof( tHeader ) );

	if( pBuffer->Write( &tHeader, sizeof( tHeader ) ) == false )	return false;
	if( pBuffer->Write( lpData, uDataSize ) == false )	return false;

	return true;
}

bool	FLIDNAuthProtocol::SendStream( CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize )
{
	//	header
	T_IDN_AUTH_HEADER tHeader;
	tHeader.SetSize( static_cast< WORD >( uDataSize ) + sizeof( tHeader ) );

	if( pQueue->Write( &tHeader, sizeof( tHeader ) ) == false )	return false;
	if( pQueue->Write( lpData, uDataSize ) == false )	return false;

	return true;
}

FLStreamProtocol*	FLIDNAuthProtocol::Clone()
{
	return this;
}

void	FLIDNAuthProtocol::Release()
{
}

void	FLIDNAuthProtocol::GetData( void* pStream, void** pData, u_long* nDataSize )
{
	T_IDN_AUTH_HEADER* pHeader = reinterpret_cast< T_IDN_AUTH_HEADER* >( pStream );
	*pData = pHeader+1;
	*nDataSize = pHeader->GetSize() - sizeof( *pHeader );
}

void*	FLIDNAuthProtocol::GetNextStream( void* pStream )
{
	T_IDN_AUTH_HEADER* pHeader = reinterpret_cast< T_IDN_AUTH_HEADER* >( pStream );
	char* pStart = reinterpret_cast< char* >( pHeader );
	return pStart + pHeader->GetSize();
}

