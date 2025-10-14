
#include "StdAfx.h"
#include "DBSerialNumberRequest.h"



CDBSerialNumberRequest *	CDBSerialNumberRequest::GetInstance()
{
	static CDBSerialNumberRequest sSerialNumberRequest;
	return &sSerialNumberRequest;
}

CDBSerialNumberRequest::CDBSerialNumberRequest()
{
	m_nRequestCount = DEFAULT_REQUEST_COUNT;
}

CDBSerialNumberRequest::~CDBSerialNumberRequest()
{
	m_nRequestCount = DEFAULT_REQUEST_COUNT;
}

void	CDBSerialNumberRequest::SetRequestCount( int nCount )
{
	if( nCount > 0 )
	{
		m_nRequestCount = nCount;
	}
}

LPDB_OVERLAPPED_PLUS	CDBSerialNumberRequest::MakeRequest()
{
	T_PACKET_SERIALNUMBER_REQ tMsg = { 0 };
	tMsg.dwCount = m_nRequestCount;

	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = g_DbManager.AllocRequest();
	g_DbManager.MakeRequest( lpDbOverlappedPlus, (LPBYTE)&tMsg, sizeof( tMsg ) );
	lpDbOverlappedPlus->dpid		= DPID_UNKNOWN;
	lpDbOverlappedPlus->dpidCache	= DPID_UNKNOWN;
	lpDbOverlappedPlus->dpidUser	= DPID_UNKNOWN;
	lpDbOverlappedPlus->nQueryMode	= QM_QUERY_SERIALNUMBER;

	return lpDbOverlappedPlus;
}

void	CDBSerialNumberRequest::Request()
{
	LPDB_OVERLAPPED_PLUS lpDbOverlappedPlus = MakeRequest();
	if( lpDbOverlappedPlus != NULL )
	{
		PostQueuedCompletionStatus( g_DbManager.m_hIOCPPut, 1, NULL, &lpDbOverlappedPlus->Overlapped );	
	}
}
