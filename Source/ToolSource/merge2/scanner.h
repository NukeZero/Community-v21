#ifndef __SCANNER_H
#define __SCANNER_H

//#include "data.h"

#ifndef NULL_ID
#define NULL_ID 0xffffffff
#endif

#define MAX_TOKENSTR 1024

/*
 *  Add additional C keyword tokens here
 */
enum Toke
{
	NONE_,ARG,VAR,IF,ELSE,FOR,DO,WHILE,BREAK,SWITCH,ANSWER,SELECT,YESNO,
	CASE,DEFAULT,GOTO,RETURN,EOL,DEFINE,INCLUDE,ENUM,FINISHED,END
};
/*
 *  Add additional double operatoes here (such as->)
 */
enum DoubleOps 
{ 
	LT = 1, LE,GT,GE,EQ,NE,NT 
};

enum TokenType 
{ 
	TEMP,DELIMITER,IDENTIFIER,NUMBER,HEX,KEYWORD,STRING,BLOCK 
};
typedef struct tagDefineData
{
	CString strDefine;
	DWORD dwData;

} DEFINEDATA,* LPDEFINEDATA;

////////////////////////////////////////////////////////////////////////////////
//
// Token : 토큰을 구분하여 읽어드림.
//
class CScanner 
{
	int GetToken_Wide( BOOL bComma );
	BOOL Read( CFileIO* pFile, BOOL bMultiByte );
	//DECLARE_SERIAL(CScanner)
	BOOL m_bComma; // 구분자가 콤마인가?
protected:

	int isdelim( wchar_t c )
	{
		// LeadByte면 delim가 아니므로 0
		if(strchr(" !:;,+-<>'/*%^=()&|\"{}",c) || c == 9 || c == '\r' || c == 0 || c == '\n' ) 
			return 1;
		return 0;
	}
	int iswhite( wchar_t c )
	{
		if(c > 0 && c <= 0x20)
			return 1;
		return 0;
	}

	BYTE m_bMemFlag; // 0은 로드, 1은 외부 포인트 
	int  m_nProgSize;
	DWORD m_dwDef;

public:
	int  GetProgSize() { return m_nProgSize; }
	BOOL m_bWideChar;
	CHAR* m_lpMark; // current location in source code
	virtual void SetMark() { m_lpMark = m_pProg; }
	virtual void GoMark() { if( m_lpMark ) m_pProg = m_lpMark; }

	CHAR* m_pProg ; // current location in source code
	CHAR* m_pBuf ; // points to start of program buffer

	CString Token;
	char*   token;//[ MAX_TOKENSTR ]  ; // string representation of token
	wchar_t m_wszToken[ MAX_TOKENSTR ]  ; // string representation of token
	char    m_mszToken[ MAX_TOKENSTR ]  ; // string representation of token
	int   tokenType   ; // contains type of token
	int   tok         ; // internal representation of token
	int   m_nDoubleOps;
	DWORD dwValue;
	TCHAR m_szFileName[ 256 ];
	BOOL m_bErrorCheck;

	void SetErrorCheck( BOOL bErrorCheck ) { m_bErrorCheck = bErrorCheck; } 
	BOOL GetErrorCheck() { return m_bErrorCheck; }

	virtual int GetLineNum( LPVOID lpProg = NULL );

	CScanner( BOOL bComma = FALSE );
	CScanner( LPVOID p, BOOL bComma = FALSE );
	~CScanner();

	void GetLastFull(); // 현재부터 끝까지 한번에 읽는다.

	// 구분자를 Comma로 지정하거나 지정 여부를 확인한다.
	void SetComma( BOOL bComma ) { m_bComma = bComma; }
	BOOL IsComma() { return m_bComma; }

	BOOL IsLoaded() { return m_pBuf == NULL ? FALSE : TRUE; }
	void Free();
	BOOL Load( LPCTSTR lpszFileName, BOOL bMultiByte = TRUE );
	BOOL Load_FileIO( LPCTSTR lpszFileName, BOOL bMultiByte = TRUE );
	void SetProg( LPVOID pProg );
	int		GetNumber( BOOL bComma = FALSE );
	__int64		GetInt64( BOOL bComma = FALSE );
//	EXPINTEGER	GetExpInteger(  BOOL bComma = FALSE )	{ return GetInt64( bComma ); }
	FLOAT GetFloat( BOOL bComma = FALSE );
	DWORD GetHex( BOOL bComma = FALSE );
	void SkipToken( int nNumber, BOOL bComma = FALSE );

	// extraction operations
	CScanner& operator>>( int& i );
	CScanner& operator>>( float& f );
	//CScanner& operator>>(LPTSTR lpszStr);

	// override
#ifdef __SCRIPT_TOOL
	int GetToken2( BOOL bComma = FALSE );
#endif 	//__SCRIPT_TOOL

virtual void PutBack();// { for(char *t = token; *t; t++) prog--; }
virtual	int GetToken( BOOL bComma = FALSE );
	
//virtual	int GetToken_Parser( BOOL bComma = FALSE ) { return GetToken( bComma ); }
//virtual	int GetToken_Def( BOOL bComma = FALSE ) { return GetToken( bComma ); }
};
#define PROGPTR( x )  m_bWideChar == FALSE ? (CHAR*) x : (wchar_t) x




#ifdef _MULTI_LANGUAGE_
/////////////////////////////////////////////////////////////////////////////

extern WORD		g_codePage;

bool	IsComposibleTh(BYTE prev, BYTE curr, int mode);
const char*	CharNextEx(const char* strText, WORD codePage = g_codePage);

/////////////////////////////////////////////////////////////////////////////
#endif //_MULTI_LANGUAGE_






#endif
