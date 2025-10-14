
#ifndef	__FLADBILLPROTOCOL_H__
#define	__FLADBILLPROTOCOL_H__


#include <network/FLStreamProtocol.h>


class	FLAdbillProtocol : public FLStreamProtocol
{
private:

	FLAdbillProtocol();
	~FLAdbillProtocol();

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