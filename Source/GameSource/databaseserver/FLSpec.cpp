#include "StdAfx.h"
#include "FLSpec.h"

#include "../_Common/file.h"
#include <MsgHdr.h>

#include "dptrans.h"


FLSpec::FLSpec()
: m_pCooperativeContributions_Buffer( NULL )
, m_nCooperativeContributions_BufferSize( 0 )
{

}

FLSpec::~FLSpec()
{
	SAFE_DELETE( m_pCooperativeContributions_Buffer );
}

FLSpec & FLSpec::GetInstance()
{
	static FLSpec inst;
	return inst;
}

BOOL FLSpec::Load_CooperativeContributions()
{
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return FALSE;

	const char * pszFileName= "Event/propCooperativeContributions_Event.inc";

	TCHAR szFullPath[ MAX_PATH ]	= { 0, };
	g_pScriptFileManager->GetScriptFileFullPath( szFullPath, _countof( szFullPath ), pszFileName );

	CFileIO kFile;
	//const CString kFileName = pszFileName;
	//kFileName.MakeLower();
	if( kFile.Open( szFullPath, "rb" ) == FALSE 
		|| kFile.GetLength() == 0 ) 
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ FAILED LOAD SCRIPT. FILE_NAME:(%s) ]" ), szFullPath );
		return FALSE;
	}

	if( m_pCooperativeContributions_Buffer != NULL || m_nCooperativeContributions_BufferSize != 0 )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ ALREADY LOAD (%s) ]" ), szFullPath );
		return FALSE;
	}

	m_nCooperativeContributions_BufferSize	= kFile.GetLength();
	m_pCooperativeContributions_Buffer		= new BYTE[ m_nCooperativeContributions_BufferSize ];

	kFile.Read( m_pCooperativeContributions_Buffer, m_nCooperativeContributions_BufferSize );
	return TRUE;
}


void FLSpec::TrySend_CooperativeContributions( const DPID dpid )
{
	//OnBaseGameSettingReq 시점에서
	if( _GetContentState( CT_DONATION ) != CS_VER1 )
		return;

	if( m_pCooperativeContributions_Buffer == NULL || m_nCooperativeContributions_BufferSize <= 0 )
		return;

	BEFORESENDDUAL( out, PACKETTYPE_COOPERATIVE_CONTRIBUTIONS_SPEC_NOTI, DPID_UNKNOWN, DPID_UNKNOWN );
	out << m_nCooperativeContributions_BufferSize;
	out.Write( m_pCooperativeContributions_Buffer, m_nCooperativeContributions_BufferSize );
	SEND( out, CDPTrans::GetInstance(), dpid );
}

