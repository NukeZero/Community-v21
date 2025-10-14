#pragma once

class FLSpec
{
public:
	FLSpec();
	virtual ~FLSpec();

	static FLSpec &			GetInstance();

	BOOL					Load_CooperativeContributions();
	void					TrySend_CooperativeContributions( const DPID dpid );

private:
	BYTE *					m_pCooperativeContributions_Buffer;
	DWORD					m_nCooperativeContributions_BufferSize;
};


