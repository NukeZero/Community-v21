
#ifndef	__FLSTREAMPROTOCOL_H__
#define	__FLSTREAMPROTOCOL_H__


#include "FLNetworkCommon.h"
#include "FLBuffer.h"


class FLStreamProtocol
{
	protected:
		FLStreamProtocol() = default;
		virtual ~FLStreamProtocol() = default;
	
	public:
	
		// recv stream
		virtual	bool FetchStream( CBuffer** pBuffer, CBufferQueue* pQueue )	= 0;
		virtual bool PushSystemStream( CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize )	= 0;
		// send stream
		virtual bool SendStream( CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize )	= 0;
	
		// creator
		virtual FLStreamProtocol* Clone() = 0;
		virtual void Release() = 0;
	
		// protocol sync
		virtual bool StartProtocol(CBufferQueue*) { return true; }
		virtual bool ReadySyncProtocol() { return true; }
		virtual bool WaitSyncProtocol(unsigned long) { return true; }
	
		// message handling
		virtual void GetData(void* pStream, void** pData, unsigned long* nDataSize)	= 0;
		virtual void* GetNextStream(void* pStream) = 0;
	
};

class FLNormalProtocol : public FLStreamProtocol
{
	FLNormalProtocol();
	~FLNormalProtocol();

public:
	bool FetchStream(CBuffer** pBuffer, CBufferQueue* pQueue) override;
	bool PushSystemStream(CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize) override;

	bool SendStream(CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize) override;

	FLStreamProtocol* Clone() override;
	void Release() override;

	void GetData( void* pStream, void** pData, u_long* nDataSize ) override;
	void* GetNextStream( void* pStream ) override;
	static FLStreamProtocol* GetInstance();

};


class FLXORProtocol : public FLStreamProtocol
{
	FLXORProtocol();
	~FLXORProtocol();
	
	unsigned long m_dwProtocolId;
	unsigned char m_byRecvIndex;
	unsigned char m_bySendIndex;
	void* m_hInitWait; //handle

	public:
		bool FetchStream(CBuffer** pBuffer, CBufferQueue* pQueue) override;
		bool PushSystemStream(CBuffer* pBuffer, LPBYTE lpData, u_long uDataSize) override;
		
		bool SendStream(CBufferQueue* pQueue, LPBYTE lpData, u_long uDataSize) override;
		
		FLStreamProtocol* Clone() override;
		void Release() override;
		
		bool StartProtocol(CBufferQueue* pQueue) override;
		bool ReadySyncProtocol() override;
		bool WaitSyncProtocol(DWORD dwTimeout) override;
		
		void GetData(void* pStream, void** pData, u_long* nDataSize) override;
		void* GetNextStream(void* pStream) override;

		static FLStreamProtocol* GetInstance();
		MEMPOOLER_DECLARE( FLXORProtocol );

};





#endif