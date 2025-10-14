
#ifndef	__FLIDNAUTHPROTOCOL_H__
#define	__FLIDNAUTHPROTOCOL_H__


#include <network/FLStreamProtocol.h>


class	FLIDNAuthProtocol : public FLStreamProtocol
{
private:

	FLIDNAuthProtocol();
	~FLIDNAuthProtocol();

public:

	bool	FetchStream( CBuffer** pBuffer, CBufferQueue* pQueue );
	bool	PushSystemStream( CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize );

	bool	SendStream( CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize );

	FLStreamProtocol*	Clone();
	void				Release();

	void		GetData( void* pStream, void** pData, u_long* nDataSize );
	void*		GetNextStream( void* pStream );


	static	FLStreamProtocol*	GetInstance();

};



#endif