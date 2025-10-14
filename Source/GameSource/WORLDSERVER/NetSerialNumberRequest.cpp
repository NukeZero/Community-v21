
#include "stdafx.h"
#include "NetSerialNumberRequest.h"



CNetSerialNumberRequest *	CNetSerialNumberRequest::GetInstance()
{
	static	CNetSerialNumberRequest	sSerialNumberRequest;
	return &sSerialNumberRequest;
}

CNetSerialNumberRequest::CNetSerialNumberRequest()
{
	m_nRequestCount = DEFAULT_REQUEST_COUNT;
}

CNetSerialNumberRequest::~CNetSerialNumberRequest()
{
	m_nRequestCount = DEFAULT_REQUEST_COUNT;
}

void	CNetSerialNumberRequest::SetRequestCount( int nCount )
{
	if( nCount > 0 )
	{
		m_nRequestCount = nCount;
	}
}

void	CNetSerialNumberRequest::Request()
{
	g_dpDBClient.SendSerialNumberReq( m_nRequestCount );
}
