#include "StdAfx.h"
#include "Query.h"

#include <crypto/FLDes.h>

void Decrpyt( unsigned char* pEncrypted, char* szResult )
{
	static unsigned char keys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

    FL_des3_context ctx3;
    des3_set_3keys( &ctx3, keys[0], keys[1], keys[2] );

    unsigned char* input = pEncrypted;
    unsigned char* result = (unsigned char*)szResult;
	for( int i=0; i<3; i++ )
		des3_decrypt( &ctx3, input + i*8, result + i*8 );
}

BOOL GetPWDFromToken( const char* szToken, char* szPWD )
{

	int nCount = strlen( szToken ) / 2;
	if( nCount > 0 )
	{
		BYTE byPWD[256];
		char szBuf[3] = {0, };
		for( int i=0; i<nCount; i++ )
		{
			memcpy( szBuf, szToken + i*2, 2 );
#if	defined(_MSC_VER) && (_MSC_VER >= 1400)
			_stscanf_s( szBuf, "%02x", (byPWD + i) );
#else
			_stscanf( szBuf, "%02x", (byPWD + i) );
#endif
		}
		Decrpyt( byPWD, szPWD );
		return TRUE;
	}
	return FALSE;
/*
	int i;
	static unsigned char keys[3][8] =
	{
		{ 0x32, 0x89, 0x45, 0x9A, 0x89, 0xAB, 0xCD, 0x11 },
		{ 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF, 0x01 },
		{ 0x1F, 0x67, 0x89, 0x54, 0xCD, 0xEF, 0x00, 0x23 }
	};

    des3_context ctx3;
    des3_set_3keys( &ctx3, keys[0], keys[1], keys[2] );

	{
		// uncomment this block to make encrypted password
		// unsigned char input[24] = { '1', '2', };       // original password
		unsigned char input[24] = {0, };       // original password
		unsigned char output[24];
		
		strcpy( (char*)input, szToken );

		for( i=0; i<3; i++ )
		{
			unsigned char buf[8] = { 0, };
			memcpy( buf, input+i*8, 8 );
			des3_encrypt( &ctx3, buf, output + i*8 );
		}

		char szBuffer[256] = {0, };
		for( i=0; i<24; i++ )
			sprintf( szBuffer+i*2, "%02X", output[i] );

		FLTRACE_LOG( PROGRAM_NAME, szBuffer );
		
	}

	return TRUE;
	*/
}

// 생성자:각종 초기화를 담당한다.
CQuery::CQuery()
{
	hStmt	= NULL;
	hDbc	= NULL;
	hDbc	= NULL;
	hEnv	= NULL;

	ParameterNumber	= 0;
	ValueLength		= SQL_NTS;

//////////////////////////////////////////////////////////////////////////
//	BEGIN100616	Data Initialize
	memset( DBName, 0, sizeof(DBName) );
	memset( DBId, 0, sizeof(DBId) );
	memset( DBPass, 0, sizeof(DBPass) );
	szTable[ 0 ]			= '\0';

	for( int i = 0; i < MAXCOL; i++ )
	{
		static const size_t	MAX_COL_STRING	= 8192;

		Col[i]	= new char[MAX_COL_STRING];
		memset( Col[i], 0, sizeof(char) * MAX_COL_STRING );
	}
//	END100616	Data Initialize
//////////////////////////////////////////////////////////////////////////
}

// 파괴자:연결 핸들을 해제한다.
CQuery::~CQuery()
{
	if (hStmt)	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);
	if (hDbc)	SQLDisconnect(hDbc);
	if (hDbc)	SQLFreeHandle(SQL_HANDLE_DBC,hDbc);
	if (hEnv)	SQLFreeHandle(SQL_HANDLE_ENV,hEnv);

	for( int i = 0; i < MAXCOL; i++ )
		SAFE_DELETE_ARRAY( Col[i] );
}

void CQuery::DisConnect()
{
	if (hStmt)	SQLFreeHandle(SQL_HANDLE_STMT,hStmt);	hStmt = NULL;
	if (hDbc)	SQLDisconnect(hDbc);
	if (hDbc)	SQLFreeHandle(SQL_HANDLE_DBC,hDbc);		hDbc = NULL;
	if (hEnv)	SQLFreeHandle(SQL_HANDLE_ENV,hEnv);		hEnv = NULL;
}



BOOL CQuery::EnableConnectionPooling()
{
	SQLRETURN result = SQLSetEnvAttr( NULL,  // make process level cursor pooling
					   			     SQL_ATTR_CONNECTION_POOLING,
						             (SQLPOINTER)SQL_CP_ONE_PER_DRIVER,
						             SQL_IS_INTEGER );

	return SQL_SUCCEEDED( result ); 
}

#ifdef __DbStuff
bool CQuery::Connect(const SqlDatabase dbType)
{
	m_dbType = dbType;
	SQLAllocHandle(SQL_HANDLE_ENV, nullptr, &hEnv);
	SQLSetEnvAttr(hEnv, SQL_ATTR_ODBC_VERSION, reinterpret_cast<SQLPOINTER>(SQL_OV_ODBC3), SQL_IS_INTEGER);
	SQLAllocHandle(SQL_HANDLE_DBC, hEnv, &hDbc);

#ifdef __PWD_CRYPT_DB
	char buffer[256]{0}; 
	::GetPWDFromToken(&dbf::password[0], buffer);
	std::string conStr = Lexer::build("Server=", &dbf::server[0], ";Driver={SQL Server Native Client 11.0};PWD=", buffer, ";UID=", &dbf::username[0], ";Database=");
#else
	std::string conStr = Lexer::build("Server=", &dbf::server[0], ";Driver={SQL Server Native Client 11.0};PWD=", &dbf::password[0], ";UID=", &dbf::username[0], ";Database=");
#endif

	switch (dbType)
	{
		case SqlDatabase::ACCOUNT:
			conStr = Lexer::build(conStr, &dbf::account[0], ";");
			break;
		case SqlDatabase::CHARACTER:
			conStr = Lexer::build(conStr, &dbf::character[0], ";");
			break;
		case SqlDatabase::LOGGING:
			conStr = Lexer::build(conStr, &dbf::logging[0], ";");
			break;
		case SqlDatabase::RANKING:
			conStr = Lexer::build(conStr, &dbf::ranking[0], ";");
			break;
		case SqlDatabase::BACKEND:
			conStr = Lexer::build(conStr, &dbf::backend[0], ";");
			break;
		case SqlDatabase::ITEM:
			conStr = Lexer::build(conStr, &dbf::itemdb[0], ";");
			break;
		case SqlDatabase::WORLD:
			conStr = Lexer::build(conStr, &dbf::world[0], ";");
			break;
		default:
			return false;
	}

	SQLRETURN ret = SQLDriverConnect(hDbc, nullptr, reinterpret_cast<SQLCHAR*>(const_cast<char*>(conStr.c_str())), SQL_NTS, nullptr, 0,
		nullptr, SQL_DRIVER_NOPROMPT);

	if (ret != SQL_SUCCESS && ret != SQL_SUCCESS_WITH_INFO)
	{
		PrintDiag(conStr.c_str());
		return false;
	}
	ret = SQLAllocHandle(SQL_HANDLE_STMT, hDbc, &hStmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO))
	{
		hStmt = nullptr;
		return false;
	}
	return true;
}
#else
BOOL CQuery::Connect(int Type, char *ConStr, char *UID, const char *PWD)
{
	FLStrcpy( DBName, _countof( DBName ), ConStr );
	FLStrcpy( DBId, _countof( DBId ), UID );
	FLStrcpy( DBPass, _countof( DBPass ), PWD );
	
	//////////////////////////////////////////////////////////////////////////
	//	BEGIN100616	Data Initialize
	SQLCHAR InCon[255] = { 0, };
	SQLCHAR OutCon[255] = { 0, };
	
    SQLSMALLINT cbOutCon	= 0;
	SQLRETURN ret	= 0;
	//	END100616	Data Initialize
	//////////////////////////////////////////////////////////////////////////

	// 환경 핸들을 할당하고 버전 속성을 설정한다.
	SQLAllocHandle(SQL_HANDLE_ENV,SQL_NULL_HANDLE,&hEnv);
	SQLSetEnvAttr(hEnv,SQL_ATTR_ODBC_VERSION,(SQLPOINTER)SQL_OV_ODBC3,SQL_IS_INTEGER);
	
	// 연결 타입에 따라 MDB 또는 SQL 서버, 또는 DSN에 연결한다.
	SQLAllocHandle(SQL_HANDLE_DBC,hEnv,&hDbc);
	switch (Type) 
	{
	case 1:
		FLSPrintf((char *)InCon, _countof( InCon ), "DRIVER={Microsoft Access Driver (*.mdb)};DBQ=%s;",ConStr);
		ret=SQLDriverConnect(hDbc,NULL,(SQLCHAR *)InCon,sizeof(InCon),OutCon,
			sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case 2:
		FLSPrintf((char *)InCon, _countof( InCon ), "FileDsn=%s", ConStr);
		ret=SQLDriverConnect(hDbc,NULL,(SQLCHAR *)InCon,sizeof(InCon),OutCon,
			sizeof(OutCon),&cbOutCon, SQL_DRIVER_NOPROMPT);
		break;
	case 3:
		ret=SQLConnect(hDbc,(SQLCHAR *)ConStr,SQL_NTS,(SQLCHAR *)UID,SQL_NTS,
			(SQLCHAR *)PWD,SQL_NTS);
		break;
	}

	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) 
	{
		PrintDiag( ConStr );
		return FALSE;
	}

	// 명령 핸들을 할당한다.
	ret=SQLAllocHandle(SQL_HANDLE_STMT,hDbc,&hStmt);
	if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO)) 
	{
		PrintDiag( ConStr );
		hStmt=0;
		return FALSE;
	}

//	::SQLSetStmtOption(hStmt, SQL_QUERY_TIMEOUT, 10);  // 10초 타임아웃 
	return TRUE;
}
#endif

BOOL	CQuery::BindParameterByte( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_UTINYINT, SQL_TINYINT, 0,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterShort( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_SSHORT, SQL_SMALLINT, 0,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterUShort( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_USHORT, SQL_SMALLINT, 0,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterInt( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_SLONG, SQL_INTEGER, 0,
		0, ParameterValuePtr, 0, &ValueLength );
	
	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}
	
	return TRUE;
}

BOOL	CQuery::BindParameterUInt( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_ULONG, SQL_INTEGER, 0,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterInt64( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_SBIGINT, SQL_BIGINT, 0,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterUInt64( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_UBIGINT, SQL_BIGINT, 0,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterFloat( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_FLOAT, SQL_REAL, 0,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterChar( SQLSMALLINT ParamType, SQLUINTEGER ColumnSize, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_CHAR, SQL_CHAR, ColumnSize,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

BOOL	CQuery::BindParameterVarChar( SQLSMALLINT ParamType, SQLUINTEGER ColumnSize, SQLPOINTER ParameterValuePtr )
{
	SQLRETURN ret = ::SQLBindParameter( hStmt, ++ParameterNumber, ParamType, SQL_C_CHAR, SQL_VARCHAR, ColumnSize,
		0, ParameterValuePtr, 0, &ValueLength );

	if( ret != SQL_SUCCESS )
	{
		FLERROR_LOG( PROGRAM_NAME, _T( "[ BIND FALIED. Result(%d), ThreadID(%d) ]" ), ret, ::GetCurrentThreadId() );
		return FALSE;
	}

	return TRUE;
}

// BOOL CQuery::BindParameter(SQLUSMALLINT parameterNumber,
//                            SQLSMALLINT inputOutputType,
//                            SQLSMALLINT valueType,
//                            SQLSMALLINT parameterType,
//                            SQLUINTEGER columnSize,
//                            SQLSMALLINT decimalDigits,
//                            SQLPOINTER  parameterValuePtr,
//                            SQLINTEGER bufferLength,
//                            SQLINTEGER *strLen_or_IndPtr)
// {
// 	SQLRETURN result = ::SQLBindParameter( hStmt, parameterNumber, inputOutputType, valueType, parameterType, columnSize,
// 		                                decimalDigits, parameterValuePtr, bufferLength, strLen_or_IndPtr );
// 	if( result != SQL_SUCCESS )
// 	{
// 		FLERROR_LOG( PROGRAM_NAME, _T( "result : %d, ThreadID : %d" ), result, ::GetCurrentThreadId() );
// 		return FALSE;
// 	}
// 	else
// 		return TRUE;
// }

// SQL문을 실행한다. 실패시 진단 정보를 출력하고 FALSE를 리턴한다.
BOOL CQuery::Exec(LPCTSTR szSQL)
{
	Clear();	// 실험적으로 항상 클리어 된 상태로 실행되게 한다.	

	SQLRETURN ret = SQLExecDirect( hStmt, (SQLCHAR*)szSQL, lstrlen( szSQL ) );
	switch( ret )
	{
	case SQL_SUCCESS_WITH_INFO:
#ifdef _DEBUG
		PrintDiag( szSQL, SQL_HANDLE_STMT );	
#endif
		// 아래 계속 실행 
	case SQL_SUCCESS:
	case SQL_NO_DATA_FOUND:
		break;
	default:
		{
			PrintDiag( szSQL, SQL_HANDLE_STMT );	
			DisConnect();
#ifdef __DbStuff
			if (Connect(m_dbType))
#else
			if( Connect( 3, DBName, DBId, DBPass ) )
#endif
			{
				ret = SQLExecDirect( hStmt, (SQLCHAR *)szSQL, SQL_NTS );
				if ((ret != SQL_SUCCESS) && (ret != SQL_SUCCESS_WITH_INFO) && (ret != SQL_NO_DATA)) 
				{
					return FALSE;
				}
			}
			else
			{
				return FALSE;
			}
		}
	}

	return PrepareFetch();
}

BOOL CQuery::MoreResults()
{
//	Clear();	// 실험적으로 항상 클리어 된 상태로 실행되게 한다.
	
	SQLRETURN ret = SQLMoreResults( hStmt );
	switch( ret ) {
	case SQL_SUCCESS:
	case SQL_SUCCESS_WITH_INFO:
		break;
	default:
		PrintDiag( "SQLMoreResults", SQL_HANDLE_STMT );	
		return FALSE;
	}

	return PrepareFetch();
}

// 결과셋에서 한 행을 가져온다.
BOOL CQuery::Fetch()
{
	SQLRETURN result = SQLFetch( hStmt );

	switch( result )
	{
	case SQL_SUCCESS_WITH_INFO:
#ifdef _DEBUG
			PrintDiag( NULL, SQL_HANDLE_STMT );
#endif
	case SQL_SUCCESS:
	case SQL_NEED_DATA:
		return TRUE;

	case SQL_NO_DATA_FOUND:		// == SQL_NO_DATA
		return FALSE;
	}

#ifdef __INTERNALSERVER
	PrintDiag( NULL, SQL_HANDLE_STMT );
#endif

	return FALSE;
}

// 커서를 닫고 바인딩 정보를 해제한다.
void CQuery::Clear()
{
	SQLCloseCursor(hStmt);
	SQLFreeStmt(hStmt, SQL_UNBIND);

	ParameterNumber		= 0;
	ValueLength			= SQL_NTS;
}

void CQuery::ClearParam()
{
	SQLFreeStmt( hStmt, SQL_RESET_PARAMS );
}

// 컬럼 이름으로부터 컬럼 인덱스를 찾는다. 없을 경우 -1을 리턴한다.
int CQuery::FindCol(char *name)
{
	int i;
	for (i=0;i<nCol;i++) 
	{
		if (_stricmp(name,(LPCTSTR)ColName[i])==0)
			return i+1;
	}
	return -1;
}

bool	CQuery::IsValidColumn( char *szName )
{
	int i = FindCol( szName );

	if( i == -1 )
	{
		return	false;
	}

	return	true;
}
// nCol의 컬럼값을 정수로 읽어준다. NULL일 경우 CQUERYNULL을 리턴한다.
int CQuery::GetInt(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA) 
	{
		return CQUERYNULL;
	} else 
	{
		return atoi(Col[nCol-1]);
	}
}

// sCol의 컬럼값을 정수로 읽어준다.
int CQuery::GetInt(char *sCol)
{
	int n;
	n=FindCol(sCol);
	if (n==-1) 
	{
		return CQUERYNOCOL;
	} else 
	{
		return GetInt(n);
	}
}

// nCol의 컬럼값을 정수로 읽어준다. NULL일 경우 CQUERYNULL을 리턴한다.
__int64 CQuery::GetInt64(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;

	if (lCol[nCol-1]==SQL_NULL_DATA) 
	{
		return CQUERYNULL;
	} else 
	{
		return _atoi64(Col[nCol-1]);
	}
}

// sCol의 컬럼값을 정수로 읽어준다.
__int64 CQuery::GetInt64(char *sCol )
{
	int n;
	n=FindCol(sCol);
	if (n==-1) 
	{
		return CQUERYNOCOL;
	} else 
	{
		return GetInt64( n );
	}
}

// nCol의 컬럼값을 실수로 읽어준다. NULL일 경우 CQUERYNULL을 리턴한다.
float CQuery::GetFloat(int nCol)
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;
	
	if (lCol[nCol-1]==SQL_NULL_DATA) 
	{
		return CQUERYNULL;
	} else 
	{
		return	( (float)( atof(Col[nCol-1]) ) );
	}
}


float CQuery::GetFloat(char *sCol)
{
	int n;
	n = FindCol(sCol);
	if (n==-1)
		return CQUERYNOCOL;
	else
		return GetFloat(n);
}


char CQuery::GetChar(int nCol)					// 실수형 컬럼 읽기
{
	if (nCol > this->nCol)
		return CQUERYNOCOL;
	
	if (lCol[nCol-1]==SQL_NULL_DATA) 
	{
		return CQUERYNULL;
	} else 
	{
		return Col[nCol-1][0];
	}
}

char CQuery::GetChar(char *sCol)				// 실수형 컬럼 읽기
{
	int n;
	n = FindCol(sCol);
	if (n==-1)
		return CQUERYNOCOL;
	else
		return GetChar(n);
}

// nCol의 컬럼값을 문자열로 읽어준다. NULL일 경우 문자열에 NULL을 채워준다. 
// buf의 길이는 최소한 256이어야 하며 길이 점검은 하지 않는다.
void CQuery::GetStr(int nCol, char *buf, size_t cchBuf)
{
	if (nCol > this->nCol) 
	{
		FLStrcpy(buf, cchBuf, "CQUERYNOCOL");
		return;
	}

	if (lCol[nCol-1]==SQL_NULL_DATA) 
	{
		FLStrcpy(buf, cchBuf, "NULL");
	} else {
		FLStrcpy(buf, cchBuf, Col[nCol-1]);
	}
}

// sCol의 컬럼값을 문자열로 읽어준다.
void CQuery::GetStr(char *sCol, char *buf, size_t cchBuf)
{
	int n;
	n=FindCol(sCol);
	if (n==-1) 
	{
		FLStrcpy(buf, cchBuf, "ERROR:Colume not found");
	} else 
	{
		GetStr(n, buf, cchBuf);
	}
}

// 에러 발생시 진단 정보를 출력해 준다.
void CQuery::PrintDiag( LPCTSTR szSQL, SQLSMALLINT type )
{

/*
	SQLINTEGER NativeError;
	SQLCHAR SqlState[6], Msg[1024];
	SQLSMALLINT MsgLen;

	int ii=1;
	SQLRETURN Ret = SQLGetDiagRec(SQL_HANDLE_STMT, hStmt, ii, SqlState, &NativeError, Msg, sizeof(Msg), &MsgLen);
	if( Ret != SQL_NO_DATA )
	{
		if( szSQL )
			WriteLogFile("query:%s\nSQLSTATE:%s error:%s", szSQL, (LPCTSTR)SqlState,(LPCTSTR)Msg );
		else
			WriteLogFile("SQLSTATE:%s error:%s", (LPCTSTR)SqlState,(LPCTSTR)Msg );
	}
*/
	UCHAR szMsg[SQL_MAX_MESSAGE_LENGTH];
	UCHAR szState[SQL_SQLSTATE_SIZE+1];
	SQLSMALLINT nLen;
	SQLINTEGER nError;
	SQLRETURN ret;

	SQLHANDLE handle = hDbc;
	if( type == SQL_HANDLE_STMT )
		handle = hStmt;

	SQLSMALLINT nIndex=1;
	while( TRUE )
	{
		ret = SQLGetDiagRec( type, handle, nIndex, szState, &nError, szMsg, SQL_MAX_MESSAGE_LENGTH-1, &nLen );
		if( (ret == SQL_SUCCESS || ret == SQL_SUCCESS_WITH_INFO) &&	lstrcmp((LPCTSTR)szState, "00000") != 0 )
		{
			if( szSQL )
				WriteLogFile("query:%s\nSQLSTATE:%s error:%s", szSQL, (LPCTSTR)szState,(LPCTSTR)szMsg );
			else
				WriteLogFile("SQLSTATE:%s error:%s", (LPCTSTR)szState,(LPCTSTR)szMsg );
			nIndex++;
		}
		else
			break;	
	}
}

// BLOB 데이터를 buf에 채워준다. 이때 buf는 충분한 크기의 메모리를 미리 할당해 
// 놓아야 한다. NULL일 경우 0을 리턴하고 에러 발생시 -1을 리턴한다. 성공시 읽은 
// 총 바이트 수를 리턴한다. szSQL은 하나의 BLOB 필드를 읽는 Select SQL문이어야 한다.
int CQuery::ReadBlob(LPCTSTR szSQL, void *buf)
{
	SQLCHAR BinaryPtr[BLOBBATCH];
	SQLINTEGER LenBin;
	char *p;
	int nGet;
	int TotalGet=0;

	SQLRETURN ret=SQLExecDirect(hStmt,(SQLCHAR *)szSQL,SQL_NTS);
	if (ret!=SQL_SUCCESS) {
		PrintDiag( szSQL );
		return -1;
	}

	while ((ret=SQLFetch(hStmt)) != SQL_NO_DATA) 
	{
		p=(char *)buf;
		while ((ret=SQLGetData(hStmt,1,SQL_C_BINARY,BinaryPtr,sizeof(BinaryPtr),
			&LenBin))!=SQL_NO_DATA) {
			if (LenBin==SQL_NULL_DATA) 
			{
				Clear();
				return 0;
			}
			if (ret==SQL_SUCCESS)
				nGet=LenBin;
			else
				nGet=BLOBBATCH;
			TotalGet+=nGet;
			memcpy(p,BinaryPtr,nGet);
			p+=nGet;
		}
	}
	Clear();
	return TotalGet;
}

// buf의 BLOB 데이터를 저장한다. szSQL은 하나의 BLOB 데이터를 저장하는 Update, Insert
// SQL문이어야 한다.
void CQuery::WriteBlob(LPCTSTR szSQL, void *buf, int size)
{
	SQLINTEGER cbBlob;
	char tmp[BLOBBATCH],*p;
	SQLPOINTER pToken;
	int nPut;

	cbBlob=SQL_LEN_DATA_AT_EXEC(size);
	SQLBindParameter(hStmt,1,SQL_PARAM_INPUT,SQL_C_BINARY,SQL_LONGVARBINARY,
		size,0,(SQLPOINTER)1,0,&cbBlob);
	SQLExecDirect(hStmt,(SQLCHAR *)szSQL,SQL_NTS);
	SQLRETURN ret=SQLParamData(hStmt, &pToken);
	while (ret==SQL_NEED_DATA) 
	{
		if (ret==SQL_NEED_DATA) 
		{
			if ((int)pToken==1) 
			{
				for (p=(char *)buf;p<(char *)buf+size;p+=BLOBBATCH) 
				{
					nPut=min(BLOBBATCH,(char *)buf+size-p);
					memcpy(tmp,p,nPut);
					SQLPutData(hStmt,(PTR)tmp,nPut);
				}
			}
		}
		ret=SQLParamData(hStmt, &pToken);
	}
	Clear();
}

void CQuery::WriteLogFile(const char* lpszFormat,...)
{
	static const TCHAR	LOG_CQUERY_LOG_FILE[]	= _T( "CQuery-LogFile" );
	TCHAR szBuffer[1024* 64];

	va_list args;
	va_start( args, lpszFormat );
	FLVSPrintf( szBuffer, _countof(szBuffer), lpszFormat, args );
	va_end( args );

	FLERROR_LOG( LOG_CQUERY_LOG_FILE, szBuffer );
}


BOOL CQuery::PrepareFetch()
{
	SQLNumResultCols(hStmt,&nCol);
	if (nCol > MAXCOL)
		return FALSE;
	
	// nCol이 0인 경우는 Select문 이외의 다른 명령을 실행한 경우이므로 
	// 바인딩을 할 필요가 없다.
	if (nCol == 0) 
	{
		Clear();
		return TRUE;
	}
	
	SWORD	nActualLen;
	SWORD	m_nSQLType;
	UDWORD	m_nPrecision;
	SWORD	m_nScale;
	SWORD	m_nNullability;
 
	// 모든 컬럼을 문자열로 바인딩해 놓는다. Col배열은 zero base, 
	// 컬럼 번호는 one base임에 유의할 것
	for (SQLUSMALLINT c=0;c<nCol;c++) 
	{
		SQLBindCol( hStmt, c+1, SQL_C_CHAR, Col[c], 8192, &lCol[c] );
		SQLDescribeCol( hStmt, c+1, ColName[c], _countof( ColName ),
			&nActualLen,
			&m_nSQLType,
			&m_nPrecision,
			&m_nScale,
			&m_nNullability );
	}

	return TRUE;
}

BOOL CQuery::PrepareQuery( const TCHAR * szQuery )
{
	Clear();

	SQLRETURN result = ::SQLPrepare( hStmt, (SQLCHAR*)szQuery, SQL_NTS);
	if( result != SQL_SUCCESS )
	{
		PrintDiag( szQuery, SQL_HANDLE_STMT );
		//FLERROR_LOG( PROGRAM_NAME, _T( "SQLPrepare :: result, (%s) " ), result,  );
		return FALSE;
	}

	return TRUE;
}

BOOL CQuery::Execute( LPCTSTR lpszFormat, ... )
{
	char szQuery[1024]		= { 0,};
	va_list	args;
	va_start( args, lpszFormat );
	int n	= FLVSPrintf( szQuery, 1024, lpszFormat, args );
	va_end( args );
	return n > 0 && Exec( szQuery );
}

BOOL CQuery::GetColumns( SQLCHAR * szTable, ColumnInfoVec & o_vecColumnInfo )
{
	const SQLRETURN ret = ::SQLColumns( hStmt, NULL, 0, NULL, 0, szTable, SQL_NTS, NULL, 0 );
    if( ret != SQL_SUCCESS )
	{
		PrintDiag( NULL, SQL_HANDLE_STMT );
		return FALSE;
	}

	o_vecColumnInfo.clear();

	SQLLEN len;

	FLColumnInfo col;
	::SQLBindCol( hStmt, 1, SQL_C_CHAR, col.szCatalog, _countof( col.szCatalog ), &len );
	::SQLBindCol( hStmt, 2, SQL_C_CHAR, col.szSchema, _countof( col.szSchema ), &len );
	::SQLBindCol( hStmt, 3, SQL_C_CHAR, col.szTableName, _countof( col.szTableName ), &len );
	::SQLBindCol( hStmt, 4, SQL_C_CHAR, col.szColumnName, _countof( col.szColumnName ), &len );
	::SQLBindCol( hStmt, 5, SQL_C_SSHORT, &col.shDataType, 0, &len );
	::SQLBindCol( hStmt, 6, SQL_C_CHAR, col.szDataTypeName, _countof( col.szDataTypeName ), &len );
	::SQLBindCol( hStmt, 7, SQL_C_SLONG, &col.nColSize, 0, &len );
	::SQLBindCol( hStmt, 8, SQL_C_SLONG, &col.nBufferLen, 0, &len );
	::SQLBindCol( hStmt, 9, SQL_C_SSHORT, &col.shDecimalDigits, 0, &len );
	::SQLBindCol( hStmt, 10, SQL_C_SSHORT, &col.shNumPrecRadix, 0, &len );
	::SQLBindCol( hStmt, 11, SQL_C_SSHORT, &col.shNullAble, 0, &len );
	::SQLBindCol( hStmt, 12, SQL_C_CHAR, col.szRemark, _countof( col.szRemark ), &len );
	::SQLBindCol( hStmt, 13, SQL_C_CHAR, col.szColumnDefault, _countof( col.szColumnDefault ), &len );
	::SQLBindCol( hStmt, 14, SQL_C_SSHORT, &col.shSqlDataType, 0, &len );
	::SQLBindCol( hStmt, 15, SQL_C_SSHORT, &col.shSqlDataTypeSub, 0, &len );
	::SQLBindCol( hStmt, 16, SQL_C_SSHORT, &col.shCharOctetLen, 0, &len );
	::SQLBindCol( hStmt, 17, SQL_C_SSHORT, &col.shOrdinalPosition, 0, &len );
	::SQLBindCol( hStmt, 18, SQL_C_CHAR, col.szIsNullAble, _countof( col.szIsNullAble ), &len );


	for( ;; )
	{
		const SQLRETURN retValue		= ::SQLFetch( hStmt );
		if( retValue == SQL_ERROR )
		{
			PrintDiag( NULL, SQL_HANDLE_STMT );
			return FALSE;
		}

		if( retValue == SQL_NO_DATA )
			break;

		o_vecColumnInfo.push_back( col );
	}


	//const size_t DB_STR_MAX = 31;

	//char szCatalog[ DB_STR_MAX ]		= { 0 };
	//char szSchema[ DB_STR_MAX ]			= { 0 };
	//char szTableName[ DB_STR_MAX ]		= { 0 };
	//char szColumnName[ DB_STR_MAX ]		= { 0 };
	//SQLSMALLINT shDataType				= 0;
	//char szDataTypeName[ DB_STR_MAX ]	= { 0 };
	//SQLINTEGER nColSize					= 0;
	//SQLINTEGER nBufferLen				= 0;
	//SQLSMALLINT shDecimalDigits			= 0;
	//SQLSMALLINT shNumPrecRadix			= 0;
	//SQLSMALLINT shNullAble				= 0;
	//char szRemark[ DB_STR_MAX ]			= { 0 };
	//char szColumnDefault[ DB_STR_MAX ]	= { 0 };
	//SQLSMALLINT shSqlDataType			= 0;
	//SQLSMALLINT shSqlDataTypeSub		= 0;
	//SQLSMALLINT shCharOctetLen			= 0;
	//SQLSMALLINT shOrdinalPosition		= 0;
 //   char szIsNullAble[ DB_STR_MAX ]		= { 0 };

	//SQLLEN len;

	//::SQLBindCol( hStmt, 1, SQL_C_CHAR, szCatalog, _countof( szCatalog ), &len );
	//::SQLBindCol( hStmt, 2, SQL_C_CHAR, szSchema, _countof( szSchema ), &len );
	//::SQLBindCol( hStmt, 3, SQL_C_CHAR, szTableName, _countof( szTableName ), &len );
	//::SQLBindCol( hStmt, 4, SQL_C_CHAR, szColumnName, _countof( szColumnName ), &len );
	//::SQLBindCol( hStmt, 5, SQL_C_SSHORT, &shDataType, 0, &len );
	//::SQLBindCol( hStmt, 6, SQL_C_CHAR, szDataTypeName, _countof( szDataTypeName ), &len );
	//::SQLBindCol( hStmt, 7, SQL_C_SLONG, &nColSize, 0, &len );
	//::SQLBindCol( hStmt, 8, SQL_C_SLONG, &nBufferLen, 0, &len );
	//::SQLBindCol( hStmt, 9, SQL_C_SSHORT, &shDecimalDigits, 0, &len );
	//::SQLBindCol( hStmt, 10, SQL_C_SSHORT, &shNumPrecRadix, 0, &len );
	//::SQLBindCol( hStmt, 11, SQL_C_SSHORT, &shNullAble, 0, &len );
	//::SQLBindCol( hStmt, 12, SQL_C_CHAR, szRemark, _countof( szRemark ), &len );
	//::SQLBindCol( hStmt, 13, SQL_C_CHAR, szColumnDefault, _countof( szColumnDefault ), &len );
	//::SQLBindCol( hStmt, 14, SQL_C_SSHORT, &shSqlDataType, 0, &len );
	//::SQLBindCol( hStmt, 15, SQL_C_SSHORT, &shSqlDataTypeSub, 0, &len );
	//::SQLBindCol( hStmt, 16, SQL_C_SSHORT, &shCharOctetLen, 0, &len );
	//::SQLBindCol( hStmt, 17, SQL_C_SSHORT, &shOrdinalPosition, 0, &len );
	//::SQLBindCol( hStmt, 18, SQL_C_CHAR, szIsNullAble, _countof( szIsNullAble ), &len );

	//for( ;; )
	//{
	//	const SQLRETURN ret		= ::SQLFetch( hStmt );
	//	if( ret == SQL_ERROR )
	//	{
	//		PrintDiag( NULL, SQL_HANDLE_STMT );
	//		return FALSE;
	//	}

	//	if( ret == SQL_NO_DATA )
	//		break;

	//	char szOut[ 1024 ]	= { 0 };
	//	FLSPrintf( szOut, _countof( szOut ), "[ szCatalog(%s) szSchema(%s) szTableName(%s) szColumnName(%s)\
	//		shDataType(%d) szDataTypeName(%s) nColSize(%d) nBufferLen(%d) shDecimalDigits(%d)\
	//		shNumPrecRadix(%d) shNullAble(%d) szRemark(%s) szColumDefault(%s) shSqlDataType(%d)\
	//		shSqlDataTypeSub(%d) shCharOctetLen(%d) shOrdinalPosition(%d) szIsNullAble(%s) ]\r\n"
	//		, szCatalog, szSchema, szTableName, szColumnName
	//		, shDataType, szDataTypeName, nColSize, nBufferLen, shDecimalDigits 
	//		, shNumPrecRadix, shNullAble, szRemark, szColumnDefault, shSqlDataType
	//		, shSqlDataTypeSub, shCharOctetLen, shOrdinalPosition, szIsNullAble );

	//	int i = 0;
	//}





	return TRUE;
}

//void CQuery::SetTable( const char * szTableName )
//{
//	if( szTableName == NULL )
//	{
//		szTable[ 0 ]		= '\0';
//		return;
//	}
//
//	FLStrcpy( szTable, _countof( szTable ), szTableName );
//}

EXPINTEGER CQuery::GetExpInteger( int nCol )
{
	return GetInt64( nCol );
}

EXPINTEGER CQuery::GetExpInteger( char* sCol )
{
	return GetInt64( sCol );
}

SERIALNUMBER CQuery::GetSerialNumber( int nCol )
{
	return GetInt( nCol );
}

SERIALNUMBER CQuery::GetSerialNumber( char* sCol )
{
	return GetInt( sCol );
}

//----------------------------------------------------------------------------------------//

void CQuery::makeFullColName( char * o_pszFullColName, const size_t uCCh, const char * szCol )
{
	if( szTable[ 0 ] != '\0' )
	{
		FLStrcpy( o_pszFullColName, uCCh, szCol );
		return;
	}

	FLStrcpy( o_pszFullColName, uCCh, szTable );
	FLStrcat( o_pszFullColName, uCCh, "." );
	FLStrcat( o_pszFullColName, uCCh, szCol );
}

int CQuery::GetIntFromTbl( char *sCol )
{
	char szFullColName[ MAX_FULL_COL ] = { 0 };
	makeFullColName( szFullColName, _countof( szFullColName ), sCol );

	return GetInt( sCol );
}

__int64 CQuery::GetInt64FromTbl( char* sCol )
{
	char szFullColName[ MAX_FULL_COL ] = { 0 };
	makeFullColName( szFullColName, _countof( szFullColName ), sCol );

	return GetInt64( sCol );
}

EXPINTEGER CQuery::GetExpIntegerFromTbl( char* sCol )
{
	char szFullColName[ MAX_FULL_COL ] = { 0 };
	makeFullColName( szFullColName, _countof( szFullColName ), sCol );

	return GetExpInteger( sCol );
}

SERIALNUMBER CQuery::GetSerialNumberFromTbl( char* sCol )
{
	char szFullColName[ MAX_FULL_COL ] = { 0 };
	makeFullColName( szFullColName, _countof( szFullColName ), sCol );

	return GetSerialNumber( sCol );
}

float CQuery::GetFloatFromTbl( char *sCol )
{
	char szFullColName[ MAX_FULL_COL ] = { 0 };
	makeFullColName( szFullColName, _countof( szFullColName ), sCol );

	return GetFloat( sCol );
}

char CQuery::GetCharFromTbl( char *sCol )
{
	char szFullColName[ MAX_FULL_COL ] = { 0 };
	makeFullColName( szFullColName, _countof( szFullColName ), sCol );

	return GetChar( sCol );
}

void CQuery::GetStrFromTbl( char *sCol, char *buf, size_t cchBuf )
{
	char szFullColName[ MAX_FULL_COL ] = { 0 };
	makeFullColName( szFullColName, _countof( szFullColName ), sCol );

	return GetStr( sCol, buf, cchBuf );
}


