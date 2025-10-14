#ifndef _QUERY_H_
#define _QUERY_H_

#include <sql.h>
#include <sqlext.h>

struct QUERY_BINDINFO
{
	SQLSMALLINT targetType;
	SQLPOINTER	targetValue;
	SQLINTEGER	bufferLength;
	SQLINTEGER*	strLen_or_Ind;
};

class CQuery
{
public:
//	char *DBName, *DBId, *DBPass;	
	char DBName[256];
	char DBId[256];
	char DBPass[256];
#ifdef __DbStuff
	enum class SqlDatabase { ACCOUNT, CHARACTER, RANKING, LOGGING, BACKEND, ITEM, SITE, WORLD };
	SqlDatabase m_dbType;
#endif 

	// 최대 컬럼수, BLOB 입출력 단위, NULL 필드값
	enum { MAXCOL=256, MAX_COLNAME = 128, BLOBBATCH=10000, CQUERYNULL=-100, CQUERYEOF=-101, 
		CQUERYNOCOL=-102, CQUERYERROR=-103
		, MAXTBL = MAX_PATH
		, MAX_FULL_COL = MAXTBL+1+MAXCOL};			//1은 '.'
private:
	SQLHENV hEnv;							// 환경 핸들
	SQLHDBC hDbc;							// 연결 핸들
	char*	Col[MAXCOL];					// 바인딩될 컬럼 정보

	int FindCol(char *name);				// 컬럼의 이름으로부터 번호를 찾아준다.


	char					szTable[ MAXTBL ];

	SQLUSMALLINT	ParameterNumber;
	SQLINTEGER		ValueLength;

//public:
//	void					SetTable( const char * szTableName );

public:
	SQLHSTMT hStmt;							// 명령 핸들. 직접 사용할 수도 있으므로 public으로 정의
	SQLSMALLINT nCol;						// 컬럼 개수
	SQLCHAR ColName[MAXCOL][MAX_COLNAME];			// 컬럼의 이름들
	SQLINTEGER lCol[MAXCOL];				// 컬럼의 길이/상태 정보

	CQuery();								// 생성자
	~CQuery();								// 파괴자:연결 핸들을 해제한다.

	void PrintDiag( LPCTSTR szSQL, SQLSMALLINT type = SQL_HANDLE_DBC );						// 진단 정보 출력

#ifdef __DbStuff
	bool Connect(SqlDatabase dbType);
#else
	BOOL Connect(int Type, char *ConStr, char *UID=NULL, const char *PWD=NULL);	// 데이터 소스에 연결한다.
#endif
	void DisConnect();						// 데이터 소스 연결을 끊는다
	BOOL Exec(LPCTSTR szSQL);				// SQL문을 실행한다.
	BOOL PrepareFetch();

	BOOL	PrepareQuery( const TCHAR * szQuery );

	BOOL	Execute( LPCTSTR lpszFormat, ... );

	int ExecGetInt(LPCTSTR szSQL);			// SQL문을 실행하고 첫번째 컬럼의 정수 읽음
	void ExecGetStr(LPCTSTR szSQL, char *buf);		// SQL문을 실행하고 첫번째 컬럼의 문자열 읽음
	BOOL Fetch();						// 한 행 가져오기
	void Clear();							// 커서 해제 및 언 바인딩
	void ClearParam();						// 파라리터 초기화



	bool					IsValidColumn( char *szName );


	int						GetInt(int nCol);					// 정수형 컬럼 읽기
	int						GetInt(char *sCol);					// 정수형 컬럼 읽기

	__int64					GetInt64( int nCol );
	__int64					GetInt64( char* sCol );

	EXPINTEGER				GetExpInteger( int nCol );
	EXPINTEGER				GetExpInteger(	char* sCol );
	SERIALNUMBER			GetSerialNumber( int nCol );
	SERIALNUMBER			GetSerialNumber( char* sCol );

	float					GetFloat(int nCol);					// 실수형 컬럼 읽기
	float					GetFloat(char *sCol);				// 실수형 컬럼 읽기
	char					GetChar(int nCol);					// 실수형 컬럼 읽기
	char					GetChar(char *sCol);				// 실수형 컬럼 읽기
	void					GetStr(int nCol, char *buf, size_t cchBuf);		// 문자열형 컬럼 읽기
	void					GetStr(char *sCol, char *buf, size_t cchBuf);		// 문자열형 컬럼 읽기

	//---------------------------------------------------------------------------------------------
	void					makeFullColName( char * szFullColName, const size_t uCCh, const char * szCol );
		

	int						GetIntFromTbl(char *sCol);					// 정수형 컬럼 읽기
	__int64					GetInt64FromTbl( char* sCol );
	EXPINTEGER				GetExpIntegerFromTbl( char* sCol );
	SERIALNUMBER			GetSerialNumberFromTbl( char* sCol );
	float					GetFloatFromTbl(char *sCol);				// 실수형 컬럼 읽기
	char					GetCharFromTbl(char *sCol);				// 실수형 컬럼 읽기
	void					GetStrFromTbl(char *sCol, char *buf, size_t cchBuf);	// 문자열형 컬럼 읽기


	int ReadBlob(LPCTSTR szSQL, void *buf);
	void WriteBlob(LPCTSTR szSQL, void *buf, int size);

	BOOL	BindParameterByte( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );

	BOOL	BindParameterShort( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );
	BOOL	BindParameterUShort( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );

	BOOL	BindParameterInt( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );
	BOOL	BindParameterUInt( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );

	BOOL	BindParameterInt64( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );
	BOOL	BindParameterUInt64( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );

	BOOL	BindParameterFloat( SQLSMALLINT ParamType, SQLPOINTER ParameterValuePtr );

	BOOL	BindParameterChar( SQLSMALLINT ParamType, SQLUINTEGER ColumnSize, SQLPOINTER ParameterValuePtr );
	BOOL	BindParameterVarChar( SQLSMALLINT ParamType, SQLUINTEGER ColumnSize, SQLPOINTER ParameterValuePtr );


// 	BOOL BindParameter(SQLUSMALLINT parameterNumber,
//                            SQLSMALLINT inputOutputType,
//                            SQLSMALLINT valueType,
//                            SQLSMALLINT parameterType,
//                            SQLUINTEGER columnSize,
//                            SQLSMALLINT decimalDigits,
//                            SQLPOINTER  parameterValuePtr,
//                            SQLINTEGER bufferLength,
//                            SQLINTEGER *strLen_or_IndPtr);
	
	BOOL MoreResults( void );

	void WriteLogFile(const char *strLog,...);
	static BOOL EnableConnectionPooling();


public:

	struct FLColumnInfo
	{
		enum				{		DB_STR_MAX = 31, 	};
		//const size_t		

		char				szCatalog[ DB_STR_MAX ];		
		char				szSchema[ DB_STR_MAX ];			
		char				szTableName[ DB_STR_MAX ];		
		char				szColumnName[ DB_STR_MAX ];		
		SQLSMALLINT			shDataType;				
		char				szDataTypeName[ DB_STR_MAX ];	
		SQLINTEGER			nColSize;					
		SQLINTEGER			nBufferLen;				
		SQLSMALLINT			shDecimalDigits;			
		SQLSMALLINT			shNumPrecRadix;			
		SQLSMALLINT			shNullAble;				
		char				szRemark[ DB_STR_MAX ];			
		char				szColumnDefault[ DB_STR_MAX ];	
		SQLSMALLINT			shSqlDataType;			
		SQLSMALLINT			shSqlDataTypeSub;		
		SQLSMALLINT			shCharOctetLen;			
		SQLSMALLINT			shOrdinalPosition;		
		char				szIsNullAble[ DB_STR_MAX ];		
	};


	typedef					std::vector< FLColumnInfo >		ColumnInfoVec;
	BOOL					GetColumns( SQLCHAR * szTable, ColumnInfoVec & o_vecColumnInfo );
};
// TODO: 모듈 분리 
// 암호화된 토큰을 해독해서 패스워드로 얻는다.
// 예: 토큰 - 3A08DB22
extern BOOL GetPWDFromToken( const char* szToken, char* szPWD );
#endif