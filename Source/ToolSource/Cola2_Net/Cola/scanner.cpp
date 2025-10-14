#include "stdafx.h"

/*************************************************************
 *													                            		 *	
 *  정수만이 가능한 Recursive descent parser		        		 *	
 *  변수의 사용이 가능하고 함수를 호출할 수 있으며,			     *
 *  goto 명령 등 Little C에서 제공하지 않는 기능을 제공한다. *
 *                            															 *			        
 *  by Travis nam			                       								 *
 *												                                	 *			
 *************************************************************/

//#include "stdafx.h"
#include <stdio.h>
#include "scanner.h"
//#include "resFile.h"

/***************
 * 토큰 스트림 *
 ***************/

inline int ishangul(char c)
{
	return c&0x80;
}

CScanner::CScanner()
{
	prog = pBuf = 0;
}
CScanner::~CScanner()
{
	Free();
}
void CScanner::Free()
{
	if(!memFlag && pBuf) 
		delete pBuf;
	prog = pBuf = 0;
}

BOOL CScanner::Load(LPCTSTR lpszFileName)
{
	int		nLength;
	FILE	*fp;

//	strlwr( (char *)lpszFileName );
	fp = fopen( lpszFileName, "rb" );
	if( fp == NULL )	return 0;
	fseek( fp, 0, SEEK_END );
	nLength = ftell( fp );
	fseek( fp, 0, SEEK_SET );
	memFlag = 0;
	prog = pBuf = new char[nLength+1];
	if(!pBuf) return 0;
	progSize = nLength;
	fread( pBuf, progSize, 1, fp );
	*(prog+nLength) = '\0';
	fclose(fp);
	//prog = pBuf;

/*	do {
		*prog = (*prog << 4) | ((BYTE)*prog >> 4);
		*prog = ~(*prog);
	} while(*(++prog));
	prog = pBuf;
*/
	return 1;
}
void CScanner::SetProg(LPTSTR ptr)
{
	memFlag = 1; prog = pBuf = ptr;
}
int CScanner::GetToken()
{
	char* temp = token; *temp = '\0';
	tokenType = TEMP; tok = NONE;
	m_nDoubleOps = 0;
BACK:
	// white space를 스킵
	while(iswhite(*prog) && *prog) 
		prog++;

	while(*prog=='/') // 주석문 처리
	{
		prog++;
		if(*prog=='/') 
		{
			prog++;
			while(*prog!='\r'&&*prog!='\0') prog++;
			if(*prog=='\r') prog+=2;
		}
		else
		if(*prog=='*') 
		{
			prog++;
			do 
			{
				while(*prog!='*') prog++;
				prog++;
			} while(*prog!='/');
			prog++;
			while(iswhite(*prog) && *prog) prog++;
		}
		else 
		{
			prog--;
			break;
		}
		goto BACK;
	}
	// 화일의 끝
	if(*prog=='\0') {
		tok = FINISHED;
		tokenType = DELIMITER; goto EXIT;
	}
	if(strchr("!<>=",*prog)) { // is or might be a relation operator
		switch(*prog) {
			case '=':
				if(*(prog+1)=='=') {
					*temp++ = *prog++;
					*temp++ = *prog++;
					m_nDoubleOps = EQ;
				}
				break;
			case '!':
				if(*(prog+1)=='=') {
					*temp++ = *prog++;
					*temp++ = *prog++;
					m_nDoubleOps = NE;
				}
				else {
					*temp++ = *prog++;
					m_nDoubleOps = NT;
				}
				break;
			case '<':
				if(*(prog+1)=='=') {
					*temp++ = *prog++;
					*temp++ = *prog++;
					m_nDoubleOps = LE;
				}
				else {
					*temp++ = *prog++;
					m_nDoubleOps = LT;
				}
				break;
			case '>':
				if(*(prog+1)=='=') {
					*temp++ = *prog++;
					*temp++ = *prog++;
					m_nDoubleOps = GE;
				}
				else {
					*temp++ = *prog++;
					m_nDoubleOps = GT;
				}
				break;
		}
		if(*token) { tokenType = DELIMITER; goto EXIT; }
	}
	// delimiter
	if(*prog == '&' && *(prog+1) == '&') {
		*temp = *(temp+1) = '&';
		temp+=2; prog+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if(*prog == '|' && *(prog+1) == '|') {
		*temp = *(temp+1) = '|';
		temp+=2; prog+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if(strchr("+-*^/%=;(),':{}.",*prog)) {
		*temp = *prog; prog++;
		temp++;	tokenType = DELIMITER; goto EXIT;
	}
	// 스트링
	if(*prog=='"') {
		prog++;
		while(*prog!='"' && *prog!='\r' && *prog!='\0') *temp++ = *prog++;
//		if(*prog=='\r'||*prog=='\0') SntxErr(SYNTAX);
		prog++; tokenType = STRING; goto EXIT;
	}
	// 숫자
	if(isdigit(*prog)&&!ishangul(*prog)) {
		while(!isdelim(*prog)) *temp++ = *prog++;
		tokenType = NUMBER; goto EXIT;
	}
	// 변수와 명령
	if(isalpha(*prog)||ishangul(*prog)||*prog=='#'||*prog=='_'||*prog=='@'||*prog=='$'||*prog=='?') {
		while(!isdelim(*prog)) *temp++ = *prog++;
	}
	else
		*temp++ = *prog++;
	tokenType = TEMP;
EXIT:
	*temp = '\0';
//	Token = token; 
	return tokenType;
}
void CScanner::PutBack()
{
	for(char *t = token; *t; t++) prog--;
}
int CScanner::isdelim(char c)
{
	if(strchr(" !:;,+-<>'/*%^=()&|\"{}",c) || c == 9 || c == '\r' || c == 0) 
		return 1;
	return 0;
}
int CScanner::iswhite(char c)
{
	if(c > 0 && c <= 0x20)
		return 1;
	return 0;
}
int CScanner::GetNumber()
{
	GetToken();
//	if(!Token.IsEmpty())
	if(token[0] != 0)
	{
		switch(token[0])
		{
			case '=': 
				return -1;
			case '-': 
				GetToken();	return -atoi(token);
				case '+':	GetToken();	
		}
		return atoi(token);
	}
	return 0;
}

// 실수 전용 읽기
float CScanner::GetNumberF()
{
	BOOL	bPlus = TRUE;
	float	fVal;
	GetToken();
	if(token[0] != 0)
	{
		switch(token[0])
		{
			case '=': 	return -1;
			case '-':	GetToken();		
						fVal = (float)-atof(token);
						return fVal;
			case '+':	GetToken();		break;
		}
		return (float)atof(token);
	}
	return 0;
}


