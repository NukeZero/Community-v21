
#include "FLStreamProtocol.h"


#define	HEADERMARK		'^'		// Normal message header



#pragma pack( 1 )
struct	T_NORMAL_HEADER
{
	BYTE	byMark;
	DWORD	dwSize;

	T_NORMAL_HEADER()
	{
		byMark = HEADERMARK;
		dwSize = 0;
	}
};
#pragma pack()


FLStreamProtocol*	FLNormalProtocol::GetInstance()
{
	static FLNormalProtocol xNormalProtocol;
	return &xNormalProtocol;
}

FLNormalProtocol::FLNormalProtocol()
{
}

FLNormalProtocol::~FLNormalProtocol()
{
}

bool	FLNormalProtocol::FetchStream( CBuffer** pBuffer, CBufferQueue* pQueue )
{
	u_long nRemnant = 0;
	LPBYTE ptr	= (*pBuffer)->GetReadableBuffer( &nRemnant );

	u_long uPacketSize = 0;

#pragma	warning( push )
#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	while( 1 )
#pragma	warning( pop )
	{
		if( nRemnant < sizeof( T_NORMAL_HEADER ) )	// 해더를 완전히 다 받지 못했다.
		{
			if( (*pBuffer)->GetBlockCount() > 0 )	// 다 받은 메시지가 있다면
			{
				CBuffer* pOld	= (*pBuffer);

				(*pBuffer)	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
				if( (*pBuffer) == NULL )
				{
					*pBuffer = pOld;
					FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 DefaultSize" ) );
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
			T_NORMAL_HEADER* pHeader = reinterpret_cast< T_NORMAL_HEADER* >( ptr );

			// header mark check
			if( pHeader->byMark != HEADERMARK )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "해더 마크 오류: Mark: %d, RecvMark: %d" ), HEADERMARK, pHeader->byMark );
				return false;
			}

			uPacketSize	= sizeof( *pHeader ) + pHeader->dwSize;
			if( nRemnant < uPacketSize )	// 데이터를 완전히 다 받지 않았다.
			{
				if( uPacketSize > (*pBuffer)->GetSize() || (*pBuffer)->GetBlockCount() > 0 )	// 버퍼가 부족하거나 받은 데이터가 있다면
				{
					CBuffer* pOld	= (*pBuffer);

					(*pBuffer)	= CBufferFactory::GetInstance().CreateBuffer( uPacketSize ); 
					if( (*pBuffer) == NULL )
					{
						*pBuffer = pOld;

						FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 Size: %d" ), uPacketSize );
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

bool	FLNormalProtocol::PushSystemStream( CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize )
{
	T_NORMAL_HEADER tHeader;
	tHeader.dwSize = uDataSize;

	if( pBuffer->Write( &tHeader, sizeof( tHeader ) ) == false )	return false;
	if( pBuffer->Write( lpData, uDataSize ) == false )	return false;

	return true;
}

bool	FLNormalProtocol::SendStream( CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize )
{
	//	header
	T_NORMAL_HEADER tHeader;
	tHeader.dwSize = uDataSize;

	if( pQueue->Write( &tHeader, sizeof( tHeader ) ) == false )	return false;
	if( pQueue->Write( lpData, uDataSize ) == false )	return false;

	return true;
}

FLStreamProtocol*	FLNormalProtocol::Clone()
{
	return this;
}

void	FLNormalProtocol::Release()
{
}

void	FLNormalProtocol::GetData( void* pStream, void** pData, u_long* nDataSize )
{
	T_NORMAL_HEADER* pHeader = reinterpret_cast< T_NORMAL_HEADER* >( pStream );
	*pData = pHeader+1;
	*nDataSize = pHeader->dwSize;
}

void*	FLNormalProtocol::GetNextStream( void* pStream )
{
	T_NORMAL_HEADER* pHeader = reinterpret_cast< T_NORMAL_HEADER* >( pStream );
	char* pStart = reinterpret_cast< char* >( pHeader );
	return pStart + sizeof( *pHeader ) + pHeader->dwSize;
}

//////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

#include "../crypto/FLCRC32.h"
#include "../crypto/FLXOR.h"

MEMPOOLER_IMPLEMENT( FLXORProtocol, 512 );



//////////////////////////////////////////////////////////////////////////
static const unsigned char FLXORPROTOCOL_KEY_TABLE[256] =
{

	0xd0, 0x36, 0x05, 0xcd,		0xa2, 0x0f, 0x00, 0xf9,		0xd7, 0x06, 0x93, 0x54,		0xbe, 0xb8, 0xea, 0xfc,
	0xd6, 0xf4, 0xfb, 0x4a,		0x34, 0x96, 0x09, 0xa8,		0xdf, 0xa5, 0x41, 0x3d,		0xd1, 0xc4, 0x6d, 0xd3,
	0xbc, 0x4d, 0xb2, 0x61,		0x1c, 0x6c, 0x61, 0x62,		0x58, 0x3a, 0x00, 0x74,		0xb5, 0x51, 0xce, 0xa3,
	0x12, 0xb7, 0xe9, 0xb9,		0xf5, 0x0f, 0xc4, 0x57,		0x88, 0x7c, 0x50, 0x8b,		0x65, 0xb0, 0xd9, 0xc6,
													   								
	0xc3, 0x0c, 0x8e, 0xa1,		0xb4, 0x0b, 0xbe, 0x37,		0x5a, 0xdc, 0x40, 0xdf,		0x5a, 0x05, 0xdf, 0x1b,
	0xd7, 0x0d, 0xd2, 0xee,		0xa0, 0x0a, 0xe2, 0x78,		0x8e, 0xe1, 0x0e, 0x98,		0x4e, 0x04, 0x83, 0x54,
	0xfb, 0xd4, 0x4c, 0x65,		0xde, 0x57, 0x29, 0x23,		0xb3, 0x66, 0x7a, 0x2e,		0xd8, 0x95, 0xd7, 0xa4,
	0xb5, 0xff, 0xe9, 0xbc,		0xa9, 0xcf, 0x7f, 0x30,		0xf2, 0x62, 0x00, 0x4e,		0xae, 0x53, 0xde, 0xbb,
													   								
	0x0b, 0xae, 0xd1, 0x6a,		0x2d, 0x02, 0xef, 0x8d,		0x37, 0xd8, 0x3b, 0xf0,		0x4a, 0xd9, 0xd6, 0x66,
	0x18, 0x78, 0x07, 0xc9,		0x39, 0x03, 0xb3, 0xc2,		0x85, 0x65, 0x30, 0xd8,		0x6b, 0x6b, 0x51, 0xf4,
	0xc4, 0x2a, 0x6f, 0x2b,		0xd4, 0xbb, 0x30, 0xe2,		0x5d, 0x68, 0x1b, 0x02,		0x94, 0x61, 0x4a, 0xb8,
	0x82, 0x08, 0xf4, 0xc1,		0x9e, 0xc5, 0x47, 0xb2,		0x1b, 0x01, 0xa5, 0x7b,		0x6c, 0x06, 0x95, 0xed,
													   								
	0x93, 0x30, 0x24, 0xb4,		0xa3, 0xa6, 0xf2, 0x1c,		0xbd, 0xbd, 0x53, 0xb3,		0xca, 0xba, 0xc2, 0x8a,
	0xd0, 0x6e, 0x77, 0xdb,		0x60, 0x16, 0xf7, 0x49,		0xa7, 0x67, 0x26, 0x61,		0x69, 0x47, 0x4d, 0x3e,
	0x08, 0xe6, 0x63, 0x5c,		0x01, 0x6d, 0x3d, 0x2d,		0x7f, 0x6a, 0x0d, 0xbb,		0x91, 0x64, 0x6c, 0x97,
	0x15, 0xda, 0x2d, 0x49,		0x8c, 0xd3, 0x7c, 0xf3,		0x62, 0xdd, 0x1d, 0xdf,		0xd9, 0x67, 0xbf, 0xba,


};
//////////////////////////////////////////////////////////////////////////


#pragma pack( 1 )
struct	T_XOR_HEADER
{
	BYTE	byMark;
	DWORD	dwSizeCrc;
	DWORD	dwSize;
	DWORD	dwDataCrc;

	T_XOR_HEADER()
	{
		byMark = HEADERMARK;
		dwSizeCrc = 0;
		dwSize = 0;
		dwDataCrc = 0;
	}
};
#pragma pack()


FLStreamProtocol*	FLXORProtocol::GetInstance()
{
	static FLXORProtocol xXORProtocol;
	return &xXORProtocol;
}


FLXORProtocol::FLXORProtocol()
{
	m_dwProtocolId = 0;
	m_byRecvIndex = 0;
	m_bySendIndex = 0;
	m_hInitWait = NULL;
}

FLXORProtocol::~FLXORProtocol()
{
	if( m_hInitWait != NULL )
	{
		SetEvent( m_hInitWait );
		m_hInitWait = NULL;
	}
}

bool	FLXORProtocol::FetchStream( CBuffer** pBuffer, CBufferQueue* pQueue )
{
	u_long nRemnant = 0;
	LPBYTE ptr	= (*pBuffer)->GetReadableBuffer( &nRemnant );

	u_long uPacketSize = 0;

	byte digest[4];

#pragma	warning( push )
#pragma warning( disable : 4127 )	// 조건식이 상수입니다.
	while( 1 )
#pragma	warning( pop )
	{
		if( nRemnant < sizeof( T_XOR_HEADER ) )	// 해더를 완전히 다 받지 못했다.
		{
			if( (*pBuffer)->GetBlockCount() > 0 )
			{
				CBuffer* pOld	= (*pBuffer);

				(*pBuffer)	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
				if( (*pBuffer) == NULL )
				{
					*pBuffer = pOld;
					FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 DefaultSize" ) );
					return false;
				}

				(*pBuffer)->Write( ptr, nRemnant );
				pOld->CutTail( nRemnant );

				pQueue->AddTail( pOld );
			}

			return true;
		}
		else	// 최소 해더 이상 받았다.
		{
			T_XOR_HEADER* pHeader = reinterpret_cast< T_XOR_HEADER* >( ptr );

			// header mark check
			if( pHeader->byMark != HEADERMARK )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "HeaderMark 실패 Mark: %d, RecvMark: %d" ), HEADERMARK, pHeader->byMark );
				return false;
			}

			// size crc check
			CRC32 crc;
			crc.Restart();
			crc.Update( (const byte*)( &pHeader->dwSize ), sizeof( pHeader->dwSize ) );
			crc.Final( digest );
			if( ~( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != pHeader->dwSizeCrc )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "SizeCrc 실패 m_dwProtocolId: %d" ), m_dwProtocolId );
				return false;
			}

			uPacketSize		= sizeof( T_XOR_HEADER ) + pHeader->dwSize;
			if( nRemnant < uPacketSize )	// 모든 데이터를 다 받지 못했다.
			{
				if( uPacketSize > (*pBuffer)->GetSize() || (*pBuffer)->GetBlockCount() > 0 )
				{
					CBuffer* pOld	= (*pBuffer);

					(*pBuffer)	= CBufferFactory::GetInstance().CreateBuffer( uPacketSize );
					if( (*pBuffer) == NULL )
					{
						*pBuffer = pOld;

						FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 Size: %d" ), uPacketSize );
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
				// 암호화 풀기
				FLXOR( m_byRecvIndex, ptr+sizeof( T_XOR_HEADER ), ptr+sizeof( T_XOR_HEADER ), pHeader->dwSize, FLXORPROTOCOL_KEY_TABLE );

				// data crc check
				crc.Restart();
				crc.Update( ptr+sizeof( T_XOR_HEADER ), pHeader->dwSize );
				crc.Final( digest );
				if( ~( *(UNALIGNED LPDWORD)digest ^ m_dwProtocolId ) != pHeader->dwDataCrc )
				{
					FLERROR_LOG( GetNetworkLogName(), _T( "DataCrc 실패 m_dwProtocolId: %d" ), m_dwProtocolId );
					return false;
				}

				//////////////////////////////////////////////////////////////////////////
				HANDLE hInitWait = m_hInitWait;
				if( m_dwProtocolId == 0 && hInitWait != NULL )
				{
					if( pHeader->dwSize == 8 )
					{
						DWORD* pProtocolID = reinterpret_cast< DWORD* >( pHeader+1 );
						if( *pProtocolID == 0x00000000 )
						{
							++pProtocolID;
							m_dwProtocolId = *pProtocolID;

							// Client
							m_byRecvIndex = HIBYTE( LOWORD( m_dwProtocolId ) );
							m_bySendIndex = LOBYTE( LOWORD( m_dwProtocolId ) );

							SetEvent( hInitWait );
							m_hInitWait = NULL;
						}
					}
				}
				//////////////////////////////////////////////////////////////////////////

				(*pBuffer)->IncBlockCount();
				nRemnant	-= ( uPacketSize );
				ptr			+= ( uPacketSize );
			}
		}
	}

	return true;
}

bool	FLXORProtocol::PushSystemStream( CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize )
{
	T_XOR_HEADER tHeader;
	tHeader.dwSize = uDataSize;

	if( pBuffer->Write( &tHeader, sizeof( tHeader ) ) == false )	return false;
	if( pBuffer->Write( lpData, uDataSize ) == false )	return false;

	return true;
}

bool	FLXORProtocol::SendStream( CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize )
{
	u_long	uRemnant	= uDataSize;
	CBuffer* pBuffer	= pQueue->GetTail();
	LPBYTE ptr;
	u_long cb;

	if( pBuffer )
	{
		cb	= pBuffer->GetWritableBufferSize();
		if( cb < sizeof( T_XOR_HEADER ) )
		{
			pBuffer	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
			if( pBuffer == NULL )
			{
				FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 DefaultSize" ) );
				return false;
			}

			pQueue->AddTail( pBuffer );
		}
	}
	else
	{
		pBuffer	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
		if( pBuffer == NULL )
		{
			FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 DefaultSize" ) );
			return false;
		}

		pQueue->AddTail( pBuffer );
	}

	ptr	= pBuffer->GetWritableBuffer( &cb );

	//	header
	T_XOR_HEADER tHeader;

	BYTE digest[sizeof(DWORD)];

	CRC32 crc;
	crc.Restart();
	crc.Update( (const byte*)( &uDataSize ), sizeof( uDataSize ) );
	crc.Final( digest );

	tHeader.dwSizeCrc = ~( (*reinterpret_cast< DWORD* >( digest )) ^ m_dwProtocolId );
	tHeader.dwSize = uDataSize;

	crc.Restart();
	crc.Update( (const byte*)( lpData ), uDataSize );
	crc.Final( digest );

	tHeader.dwDataCrc = ~( (*reinterpret_cast< DWORD* >( digest )) ^ m_dwProtocolId );

	pBuffer->Write( &tHeader, sizeof( tHeader ) );
	cb -= sizeof( tHeader );

	//	data
	cb	= ( cb < (int)uRemnant? cb: uRemnant );

	// 암호화
	FLXOR( m_bySendIndex, &ptr[sizeof( T_XOR_HEADER )], lpData, cb, FLXORPROTOCOL_KEY_TABLE );

	pBuffer->UseTail( cb );

	uRemnant	-= cb;
	while( uRemnant > 0 )
	{
		pBuffer	= CBufferFactory::GetInstance().CreateBuffer( MAX_BUFFER );
		if( pBuffer == NULL )
		{
			FLERROR_LOG( GetNetworkLogName(), _T( "버퍼 할당 실패 DefaultSize" ) );
			return false;
		}

		pQueue->AddTail( pBuffer );

		ptr	= pBuffer->GetWritableBuffer( &cb );
		cb	= ( cb < (int)( uRemnant ) ? cb: uRemnant );

		// 암호화
		FLXOR( m_bySendIndex, ptr, &lpData[uDataSize - uRemnant], cb, FLXORPROTOCOL_KEY_TABLE );

		pBuffer->UseTail( cb );
		uRemnant	-= cb;
	}

	return true;
}

bool	FLXORProtocol::StartProtocol( CBufferQueue* pQueue )
{
	if( m_hInitWait == NULL )
	{
		DWORD dwProtocolId	= GetTickCount();
		char lpData[8];
		*( (UNALIGNED DWORD*)lpData )	= 0x00000000;
		*( (UNALIGNED DWORD*)&lpData[4] )	= dwProtocolId;

		if( SendStream( pQueue, (LPBYTE)lpData, sizeof( lpData ) ) == false )
		{
			FLERROR_LOG( GetNetworkLogName(), _T( "SendStream() 실패" ) );
			return false;
		}

		m_dwProtocolId = dwProtocolId;

		// Server
		m_byRecvIndex = LOBYTE( LOWORD( m_dwProtocolId ) );
		m_bySendIndex = HIBYTE( LOWORD( m_dwProtocolId ) );
	}

	return true;
}

bool	FLXORProtocol::ReadySyncProtocol()
{
	m_hInitWait = CreateEvent( NULL, FALSE, FALSE, NULL );
	if( m_hInitWait == NULL )
	{
		FLERROR_LOG( GetNetworkLogName(), _T( "SendStream() m_hInitWait == NULL" ) );
		return false;
	}

	return true;
}

bool	FLXORProtocol::WaitSyncProtocol( DWORD dwTimeout )
{
	HANDLE hTempHandle = m_hInitWait;
	if( hTempHandle != NULL )
	{
		DWORD dwRet = WaitForSingleObject( hTempHandle, dwTimeout );
		CLOSE_HANDLE( hTempHandle );

		if( dwRet != WAIT_OBJECT_0 )
		{
			FLERROR_LOG( GetNetworkLogName(), _T( "SendStream() dwRet != WAIT_OBJECT_0" ) );
			return false;
		}
	}

	return true;
}

FLStreamProtocol*	FLXORProtocol::Clone()
{
	return new FLXORProtocol();
}

void	FLXORProtocol::Release()
{
	if( this != GetInstance() )
	{
		delete this;
	}
}

void	FLXORProtocol::GetData( void* pStream, void** pData, u_long* nDataSize )
{
	T_XOR_HEADER* pHeader = reinterpret_cast< T_XOR_HEADER* >( pStream );
	*pData = pHeader+1;
	*nDataSize = pHeader->dwSize;
}

void*	FLXORProtocol::GetNextStream( void* pStream )
{
	T_XOR_HEADER* pHeader = reinterpret_cast< T_XOR_HEADER* >( pStream );
	char* pStart = reinterpret_cast< char* >( pHeader );
	return pStart + sizeof( *pHeader ) + pHeader->dwSize;
}
