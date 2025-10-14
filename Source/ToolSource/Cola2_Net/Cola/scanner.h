#ifndef __SCANNER_H
#define __SCANNER_H

#include "windows.h"

/*
 *  Add additional C keyword tokens here
 */
enum Toke
{
	NONE,ARG,VAR,IF,ELSE,FOR,DO,WHILE,BREAK,SWITCH,ANSWER,SELECT,YESNO,
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
	TEMP,DELIMITER,IDENTIFIER,NUMBER,KEYWORD,STRING,BLOCK 
};

////////////////////////////////////////////////////////////////////////////////
//
// Token : 토큰을 구분하여 읽어드림.
//
class CScanner
{
protected:

	int isdelim(char c);
	int iswhite(char c);

	BYTE memFlag; // 0은 로드, 1은 외부 포인트 
	int  progSize;

public:

	LPTSTR prog ; // current location in source code
	LPTSTR pBuf ; // points to start of program buffer

	char token[256]  ; // string representation of token
	char tokenType   ; // contains type of token
	char tok         ; // internal representation of token
	int  m_nDoubleOps;
	CScanner(); ~CScanner();

	BOOL IsLoaded() { return pBuf == NULL ? FALSE : TRUE; }
	void Free();
	BOOL Load(LPCTSTR lpszFileName);
	void SetProg(LPTSTR initProg);
	void PutBack();
	int  GetNumber();
	float GetNumberF();

	// override

virtual	int GetToken();
};
#endif
