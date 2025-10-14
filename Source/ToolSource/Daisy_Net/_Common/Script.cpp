/*************************************************************
 *													                            		 *	
 *  정수만이 가능한 Recursive descent parser		        		 *	
 *  변수의 사용이 가능하고 함수를 호출할 수 있으며,			     *
 *  goto 명령 등 Little C에서 제공하지 않는 기능을 제공한다. *
 *                            															 *			        
 *  by Travis nam			                       								 *
 *												                                	 *			
 *************************************************************/

#include "stdafx.h"

#pragma warning ( disable : 4996 )

#ifdef __CLIENT
#include "Mover.h"
extern CMover*          g_pPlayer     ; // 플레이어 객체 
#endif // __CLIENT

Command table[] = 
{ 
	" "       ,NONE_    ,
	" "       ,ARG     ,
	"int"     ,VAR     , 
	"if"      ,IF      , 
	"else"    ,ELSE    , 
	"for"     ,FOR     , 
	"do"      ,DO      , 
	"while"   ,WHILE   , 
	"break"   ,BREAK   ,
	"switch"  ,SWITCH  , 
	"answer"  ,ANSWER  ,
	"Select"  ,SELECT  ,
	"YesNo"   ,YESNO   ,
	"case"    ,CASE    , 
	"default" ,DEFAULT ,
	"goto"    ,GOTO    , 
	"return"  ,RETURN  , 
	" "       ,EOL     , 
	"#define" ,DEFINE  , 
	"#include",INCLUDE , 
	"enum"	  ,ENUM    , 
	" "       ,FINISHED,
	0         ,END      // 테이블의 끝을 표시
};

int CopyChar( CHAR** ppSrc, CHAR** ppDst )
{
	char* pNext = CharNextExA( CP_ACP, *ppSrc, 0 );
	int nNum, nCount = nNum = (int) (pNext - *ppSrc);
	while( nCount-- )
		*(*ppDst)++ = *(*ppSrc)++;
	return nNum;
}

/****************************
 * 스크립트 파서            * 
 ****************************/

void CParser::SntxErr( LPCTSTR lpszHeader, int error)
{
	throw "fuck";
	//longjmp(eBuf,1); // 저장해둔 처음 포인터로 돌아간다
}
void CParser::operator () (LPTSTR initProg,int* value)
{
	Compute(initProg,value);
}
void CParser::operator () (int* value)
{
	Compute(value);
}
void CParser::Compute(LPTSTR initProg,int *value)
{
	SetProg(initProg);
	Compute(value);
}
void CParser::Compute(int *value)
{
	GetToken();
/*
	TRY
		if(!*token) 
			SntxErr( NULL, NO_EXP);
		else
		{
			EvalExp0(value);
			PutBack(); // return last token read to input stream 
		}
		*/
	//AND_CATCH_ALL(e)
		//TRACE("Throw Exception\n");
	//END_CATCH_ALL
		
}
void CParser::EvalExp0(int *value)
{
	EvalExp1(value);
}
void CParser::EvalExp1(int *value)
{
	int partial_value; char op,op2;
	EvalExp2(value); op = *token; op2 = *(token+1);
	while(((op = *token) == '&' && (op2 = *(token+1)) == '&') || (op == '|' && op2 == '|')) 
	{
		GetToken();	EvalExp2(&partial_value);
		switch(op) 
		{
			case '&': *value = *value && partial_value; break;
			case '|': *value = *value || partial_value; break;
		}
	}
}
void CParser::EvalExp2(int *value)
{
	//static char relops[] = { LT,LE,GT,GE,EQ,NE,NT,0 };
	int partial_value;
	register char op;

	EvalExp3(value);
	op = m_nDoubleOps;
	if(op) { // && strchr(relops,op)) {
		GetToken();	EvalExp3(&partial_value);
		switch(op) { /* perform the relational operation */
			case LT: *value = *value <  partial_value; break;
			case LE: *value = *value <= partial_value; break;
			case GT: *value = *value >  partial_value; break;
			case GE: *value = *value >= partial_value; break;
			case EQ: *value = *value == partial_value; break;
			case NE: *value = *value != partial_value; break;
			case NT: *value = !(partial_value); break;
		}
	}
}
void CParser::EvalExp3(int *value)
{
	register char op;
	int partial_value;

	EvalExp4(value);
	while((op = *token) == '+' || op == '-') {
		GetToken();
		EvalExp4(&partial_value);
		switch(op) { /* add or subtract */
			case '-':
				*value = *value - partial_value;
				break;
			case '+':
				*value = *value + partial_value;
				break;
		}
	}
}
void CParser::EvalExp4(int *value)
{
	char op;
	int partial_value;

	EvalExp5(value);
	while((op = *token) == '*' || op == '/' || op == '%') {
		GetToken();
		EvalExp5(&partial_value);
		switch(op) { /* mul, div, or modulus */
			case '*':
				*value = *value * partial_value;
				break;
			case '/':
				*value = (*value) / partial_value;
				break;
			case '%':
				//t = (*value) / partial_value;
				//*value = *value - (t * partial_value);
				*value = (*value) % partial_value;
				break;
		}
	}
}
void CParser::EvalExp5(int *value)
{
	char op;

	op = '\0';
	if(*token=='+'||*token=='-') {
		op = *token;
		GetToken();
	}
	EvalExp6(value);
	if(op)
		if(op=='-') *value = -(*value);
}
void CParser::EvalExp6(int *value)
{
	if(*token=='(') {
		GetToken();
		EvalExp0(value); /* get subexpression */
		if(*token!=')') SntxErr( NULL, PAREN_EXPECTED);
		GetToken();
	}
	else atom(value);
}
void CParser::atom(int *value)
{
	switch(tokenType) {
		case NUMBER: /* is numeric constant */
			*value = atoi(token);
			GetToken();
			return;
		default:
			if(*token!=')')    // Process empty expression 
				SntxErr( NULL, SYNTAX); // syntax error 
	}
}
/****************************
 * 스크립트 인터프리터      *
 ****************************/

extern InterFuncType interFunc[];

CScript::CScript( BOOL bComma )
{
	process = INTERPRITER;
	m_pOldProg       = 0;
	functos       = 0; // index to top of function call stack
	funcIndex     = 0; // index to into function table
	gVarIndex     = 0; // index into global variable table
	lvartos       = 0; // index into local variable stack
	labeltos      = 0; // index into local label table
	retValue      = 0; // function return value
	blocktos      = 0; //
	answerCnt     = 0;
	labelIndex    =-1;
	retSign	      = 0;
	m_pDefineBuff = 0;
	SetComma( bComma );
#ifdef __V050322_CACHEQUEST
	m_paTimer = NULL;
	ZeroMemory( m_anInternalVal, sizeof( m_anInternalVal ) );
#else
	memset(m_globalArray,0,MAX_GLOBAL_ARRAY*sizeof(int));
#endif
}
CScript::~CScript()
{
	Free();
}
#ifndef __V050322_CACHEQUEST
BOOL CScript::m_nRunScript = 0;
BOOL CScript::m_bPressDialog = FALSE;
BOOL CScript::m_bQuitDialog = FALSE;
BOOL CScript::m_bEnableDialog = FALSE;
BOOL CScript::m_bWaitServerResponse = FALSE;
#endif
BOOL	CScript::m_bKeepGlobalDef	= FALSE;

void CScript::SntxErr( LPCTSTR lpszHeader, int error)
{

	if( m_bErrorCheck == FALSE )
		return;
	SetMark();
	PutBack();
	BYTE str[80];
	int nLen = strlen(m_pProg);
	if(nLen > 79) nLen = 79;
	int i = 0;
	for(; i < nLen; i++)
		str[i] = m_pProg[i];
	str[i] = 0; 
	CString string; 
	if( lpszHeader )
		string.Format( "FileName %s(%d) : %s : %s", m_szFileName, GetLineNum(), lpszHeader, str );
	else
		string.Format( "FileName %s(%d) : %s", m_szFileName, GetLineNum(), str );

#	ifndef __SFXEDITOR
	Error( string );//endif	// !( __BEAST|| __WORLDSERVER )
	GoMark();
#	endif
}
int CScript::GetLineNum( LPTSTR lpProg )
{
	if( m_pOldProg == NULL )
		return CScanner::GetLineNum();
	return CScanner::GetLineNum( m_pOldProg );
	/*
	CHAR* pTemp = m_pDefineBuff;
	int nLine = 0;
	while( m_pProg != pTemp )
	{
		if( *pTemp == '\r' )
			nLine++;
		pTemp++;
	}
	return nLine;
	*/
}
void CScript::GetLastFull()
{
	if( m_pOldProg == NULL )
		m_strDefine = "";
	
	CScanner::GetLastFull();	
	cmd = 0;
	
	if( *token == 0 && m_pOldProg ) 
	{
		m_pProg = m_pOldProg; 
		m_pOldProg = 0;
		m_bWideChar = m_bOldWideChar;
		GetLastFull();	
	}
}
int CScript::GetTkn( BOOL bComma )
{
	CScanner::GetToken( bComma ); cmd = 0;

	if( tokenType == DELIMITER ) 
	{
		if( *token == '{' || *token == '}' ) // 블럭 처리
			tokenType = BLOCK;
	}
	else
	if( tokenType == TEMP ) 
	{
		if( tok = LookUp( token ) ) 
			tokenType = KEYWORD;
		else 
			tokenType = IDENTIFIER;
	}
	return tokenType;
}
int CScript::GetToken( BOOL bComma )
{
	if( m_pOldProg == NULL )
		m_strDefine = "";

	GetTkn( bComma );
#ifdef __USE_STR_TBL0407
	m_strKey	= "";
	string str;
	map<string, string>::iterator i = CScript::m_mapString.find( (LPCSTR)Token );
	if( i != CScript::m_mapString.end() )
	{
		tokenType = STRING;
		m_strKey	= Token;
		lstrcpy( m_mszToken, i->second.data() );
		Token	= m_mszToken;
		_mbstowcsz( m_wszToken, m_mszToken, lstrlen( m_mszToken ) + 1 );
	}
#endif	// __USE_STR_TBL0407
	if( tokenType == IDENTIFIER ) 
	{
		// 인터프리터 모드에서만 실행 
		if( IsInterpriterMode() )
		{
			char* pData = NULL;
			if( LookupDefine( token, (void*&)pData ) )//||  m_localDef.Lookup( token, (void*&)pData ) )
			{
				m_strDefine = token;
				m_pOldProg = m_pProg; 
				m_pProg = pData;
				m_pDefineBuff = pData;
				m_bOldWideChar = m_bWideChar;
				m_bWideChar = FALSE;
				tokenType = GetToken();
				// GetToken한다고 해도 define 내의 모든 스트링을 얻지는 못했다.
				// 내용이 더 있을 수 있으므로 여기서 m_pProg를 다시 복구시키면 읽다 만 결과가 된다.
				//m_pProg = m_pOldProg; m_pOldProg = 0;
			}
			else
			{
				if( m_dwDef == 1 )
				{
					// lookup을 못한 키는 변수일 수도 있고 함수일 수도 있다.
					if( *token != '\0' && *token != '=' && *token != '-' && *token != '+' ) // 프로퍼티 때문에. =을 -1로 사용하는 것은 잠정적으로 문제가 있음.
					{
						CString string;
						string.Format( "%s Not Found.", token );
						//AfxMessageBox( string );
						//SetMark();
						SntxErr( string, 0 );
						//GoMark();
					}
				}
				else
				if( m_dwDef == 2 )
				{
					// 키워드도 아니고, 내장함수도 아니고, 유저함수도 아니라면 도대체 뭐지?
					if( !( tok = LookUp( token ) ) && InternalFunc( token ) == -1 && FindFunc( token ) == FALSE && *token != '#' )
					{
						CString string;
						string.Format( "%s Not Found.", token );
						//AfxMessageBox( string );
						SntxErr( string, 0 );
					}
				}
			}
		}
		else
		{
			//SntxErr( token, 0 );
		}
	}
	else
	if( tok == FINISHED && m_pOldProg ) 
	{
		m_pProg = m_pOldProg; 
		m_pOldProg = 0;
		m_bWideChar = m_bOldWideChar;
		return GetToken( bComma );
	}
	return tokenType;
}
void CScript::SetMark()
{
	CScanner::SetMark();
	// Is in Define?
	if( m_pOldProg )
		m_bMarkInDefine = TRUE;
	else
		m_bMarkInDefine = FALSE;
}
void CScript::GoMark()
{
	// define 
	if( m_bMarkInDefine == TRUE )
	{
		if( m_pOldProg )
		{
		}
		else
		{
			m_pOldProg = m_pProg; 
			m_bOldWideChar = m_bWideChar;
			m_bWideChar = FALSE;
		}
	}
	else
	{
		if( m_pOldProg )
		{
			m_pOldProg = NULL;
			m_bWideChar = m_bOldWideChar;
		}
		else
		{
		}

	}
	CScanner::GoMark();
}
#if 0 
int CScript::GetToken_Def( BOOL bComma )
{
	if( m_pOldProg == NULL )
		m_strDefine = "";

	//GetTkn( bComma );
	CScanner::GetToken( bComma ); 
	
	cmd = 0;
	
	if( tokenType == DELIMITER ) 
	{
		if( *token == '{' || *token == '}' ) // 블럭 처리
			tokenType = BLOCK;
	}
	else
	if( tokenType == TEMP ) 
	{
		//else tokenType = IDENTIFIER;
		// comma일 경우는, NUMBER와 TEMP 구분 밖에 없기 때문에  define 추정 스트링으로 간주하고 조사하자.
		if( bComma || IsComma() )
		{
			tok = LookUp( token ); /* convert to internal rap */
			if( tok ) tokenType = KEYWORD; /* is a keyword */
			//else tokenType = IDENTIFIER;
			
			TCHAR* pTemp = token;
			while( isalpha( *pTemp ) || isdigit( *pTemp ) || *pTemp=='_' ) pTemp++;
			if( strlen( token ) == (int)( pTemp - token ) )
				tokenType = IDENTIFIER;
			//else
			//	tokenType = KEYWORD;
				
		}
		else
		{
			tok = LookUp( token ); /* convert to internal rap */
			if( tok ) tokenType = KEYWORD; /* is a keyword */
			else tokenType = IDENTIFIER;
		}			
	}
	if( tokenType == IDENTIFIER ) 
	{
		// 인터프리터 모드에서만 실행 
		if( IsInterpriterMode() )
		{
			char* pData = NULL;
			if( LookupDefine( token, (void*&)pData ) )
			{
				m_strDefine = token;
				m_pOldProg = m_pProg; m_pProg = pData;
				tokenType = GetToken_Def();
				// GetToken한다고 해도 define 내의 모든 스트링을 얻지는 못했다.
				// 내용이 더 있을 수 있으므로 여기서 m_pProg를 다시 복구시키면 읽다 만 결과가 된다.
				//m_pProg = m_pOldProg; m_pOldProg = 0;
			}
			/*
			else
			{
				// lookup을 못한 키는 변수일 수도 있고 함수일 수도 있다.
				if( *token != '\0' && *token != '=' ) // 프로퍼티 때문에. =을 -1로 사용하는 것은 잠정적으로 문제가 있음.
				{
					CString string;
					string.Format( "스크립트 오류 : %s 파일에서 %s를 찾을 수 없습니다.", m_szFileName, token );
					//AfxMessageBox( string );
					SetMark();
					SntxErr( string, 0 );
					GoMark();
				}
			}
			*/
		}
	}
	else
	if( tok == FINISHED && m_pOldProg ) 
	{
		m_pProg = m_pOldProg; 
		m_pOldProg = 0;
		m_bWideChar = m_bOldWideChar;
		return GetToken_Def( bComma );
	}
	return tokenType;
}
#endif
int CScript::GetToken_NoDef( BOOL bComma )
{
	if( m_pOldProg == NULL )
		m_strDefine = "";

	GetTkn( bComma );
	if( tok == FINISHED && m_pOldProg ) 
	{
		m_pProg = m_pOldProg; 
		m_pOldProg = 0;
		m_bWideChar = m_bOldWideChar;
		return GetTkn( bComma );
	}
	return tokenType;
}
char CScript::LookUp(char *s)
{
	if(!process) // 인터프리터 
	{
		for(int i = 0; *table[i].command; i++)
			if(!strcmp(table[i].command,s)) 
				return cmd = table[i].tok;
	}
	else // 컴파일
	if(s[0]=='@') 
		return table[atoi(&s[1])].tok;
	return 0; 
}
int CScript::InternalFunc(char *s)
{
	if(!process) // 인터프리터
	{
		for(int i = 0; interFunc[i].p; i++)
			if(!strcmp(interFunc[i].name,s))
				return i;
	}
	else // 컴파일 
	if(s[0]=='$') 
		return atoi(&s[1]);
	return -1;
}
void CScript::Compute(int *value)
{
	GetToken();
	if(!*token) 
		SntxErr( NULL, NO_EXP);
	else
	{
		EvalExp0(value);
		PutBack(); /* return last token read to input stream */
	}
}
void CScript::EvalExp0(int *value)
{
	char temp[ID_LEN]; /* holds name of val receiving the assignment */
	register int temp_tok;

	if(tokenType == IDENTIFIER) 
	{
		if(IsVar(token)) 
		{ /* if a var, see if assignment */
			strcpy(temp,token);
			temp_tok = tokenType;
			GetToken();
			char op;
			op =  *token;
			if( ( op=='+' ||op=='-' || op=='|' || op=='&' ) && *m_pProg == '=' ) 
			{
				GetToken();
				GetToken();
				EvalExp0(value); /* get value to assign */
				int *pnValue = GetVarPtr( temp );
				if( pnValue )
				{
					switch( op )
					{
					case '+': *pnValue += *value; break; // += 처리 
					case '-': *pnValue -= *value; break; // -= 처리 
					case '*': *pnValue *= *value; break; // *= 처리 
					case '/': *pnValue /= *value; break; // /= 처리 
					case '|': *pnValue |= *value; break; // |= 처리 
					case '&': *pnValue &= *value; break; // &= 처리 
					}
				}
			}
			else
			if(*token=='=' && *(token+1)!='=') 
			{ /* is an assignment */
				GetToken();
				EvalExp0(value); /* get value to assign */
				AssignVar(temp,*value); /* assign the value */
				return;
			}
			else 
			{ /* not an assignment */
				PutBack(); /* restore original token */
				strcpy(token,temp);
				tokenType = temp_tok;
			}
		}
	}
	EvalExp1(value);
}
void CScript::atom(int *value)
{
	int i;
	switch(tokenType) 
	{
		case IDENTIFIER:
			i = InternalFunc(token);
			if(i!=-1) 
			{ /* Call "standard libray" function */
				retValue = (*interFunc[i].p)(this);
				*value = retValue;
			}
			else
			if(FindFunc(token)) 
			{ /* Call user-defined function */
				Call();
				*value = retValue;
			}
			else *value = FindVar(token); /* fet var's value */
			GetToken();
			return;
		case NUMBER: /* is numeric constant */
			*value = atoi(token);
			GetToken();
			return;
		case DELIMITER: /* see if cahracter constant */
			if(*token=='\'') 
			{
				if( m_bWideChar == FALSE )
				{
					CHAR* pProg = (CHAR*)m_pProg;
					*value = *pProg;
					pProg++;
					if(*pProg!='\'') SntxErr( NULL, QUOTE_EXPECTED);
					pProg++;
					m_pProg = pProg;
				}
				GetToken();
			}
			return;
		default:
			if(*token!=')')    // Process empty expression 
				SntxErr( NULL, SYNTAX); // syntax error 
	}
}
/*
 *  return value
 *  0 == block end ( { } ), one line end( ; )
 *  1 == goto 
 *  2 == break
 *  3 == return
 */
int CScript::InterpBlock(int aaa)
{
	static MSG msg;
	int value,c,brk = 0;
	char block = 0;
	char tpTkn[64];
	CHAR* p;

	if(aaa) 
	{
		block = 1; blocktos++;
	}
	do {
		tokenType = GetToken();

		/* if interpreting single statement, return on first semicolon */

		/* see what kind of token is up */
		if(tokenType==IDENTIFIER) 
		{
			p = m_pProg;	
			strcpy( tpTkn, token );
			GetToken();
			if(*token!=':') 
			{
				strcpy(token,tpTkn);
				m_pProg = p;
				/* not a keyword, so process expression */
				PutBack(); /*  restore token to input stream
							 for further processing by Compute() */
				Compute(&value); /* process the expression */
				GetToken();
				if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
			}
		}
		else
		if(tokenType==BLOCK) 
		{ /* If block delimiter */
			if(*token=='{') 
			{    /* is a block */
				block = 1;         /* interpreting block, not statement */
				blocktos++;
			}
			else 
			{
				blocktos--;
				return 0;          /* is a }, so return */
			}
		}
		else 									 /* is keyword */
		switch(tok) 
		{
			case VAR: 	 /* declare local variables */
				PutBack();
				DeclLocal();
				break;
			case DEFINE:
				ExecDefine();
				break;
			case RETURN: /* return from function call */
				FuncRet(); 
				retSign = 1;
				break;
			case IF: /* process an if statement  */
				ExecIf();
				break;
			case ELSE:   /* process an else statement */
				FindEob(); /* find end of else block and continue execution */
				break;
			case WHILE:  /* process a while loop */
				ExecWhile();
				break;
			case DO:     /* process a do-while loop */
				ExecDo();
				break;
			case FOR:
				ExecFor();
				break;
			case BREAK:
				if( block ) blocktos--;
				return 2;
			case DEFAULT:
			case CASE:
				do GetToken(); while(*token!=':');
				break;
			case SWITCH:
				ExecSwitch();
				break;
				/*
			case ANSWER:
				ExecAnswer();
				break;
			case SELECT:
				temp = ExecSelect();
				for(c = callStack[functos-1].label; c < labeltos; c++)
					if(!strcmp(labelTable[c].name,temp)) { labelIndex = c; break; }
				break;
			case YESNO:
				temp = ExecYesNo();
				for(c = callStack[functos-1].label; c < labeltos; c++)
					if(!strcmp(labelTable[c].name,temp)) { labelIndex = c; break; }
				break;
				*/
			case GOTO:
				GetToken();
				for(c = callStack[functos-1].label; c < labeltos; c++)
					if(!strcmp(labelTable[c].name,token)) { labelIndex = c; break; }
				break;
			case END:
				exit(0);
		}
		if( retSign ) 
		{
			// retSign은 블럭으로 리커시브 된 상태를 계속 리턴하게 해준다.
			// 리커시브 상태의 서브 함수에서 return한다. 코드 3
			if( blocktos-- != 1 )	
				return 3;
			// 모든 블럭의 재귀 호출이 리턴되면 retSign을 0으로 리셋한다.
			retSign = 0; 
			return 0;
		}
		if( labelIndex!=-1 ) 
		{
			if( !block ) 
				return 1;
			if( labelTable[labelIndex].block < blocktos ) 
			{
				blocktos--; 
				return 1;
			}
			else 
			{
				m_pProg = labelTable[labelIndex].loc;
				labelIndex = -1;
			}
		}
		//
		// 멀티 태스킹이 가능하게 한다.
		//
		/*
		if(::PeekMessage(&msg,NULL,0,0,PM_REMOVE)) 
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
		*/
	} while( tok != FINISHED && block );
	return 0;
}
void CScript::PreScan( BOOL bLocalDef )
{
	char* p, temp[32], fileName[32];
	int eNum, brace = 0; // when 0, this var tells us that currnt source
                                  // position is outside of any function 
	funcIndex = 0;

	//m_globalDef.AddDefineMap(lpDefineFileName,nDefGlobal);
	p = m_pProg;

	GetTkn();
	if(!strcmp(token,"#"))
	{
		process = COMPILER;
		// 땜빵 코드다. 만약 실행할 스크립트가 하나라도 컴파일 모드라면 
		// define을 사용하지 않는 것으로 가정한다. 따라서 글로벌 define 정의도 모두 파괴한다.
		// 이것은 사실, 프로그램 초기화때 결정해야하나, 서버와 클라이언트간 상태 동기가 
		// 실행시에는 알 수 없으므로, 서버가 인터프리터 모드인데 클라이언트에서 글로벌 정의를
		// 파괴하면 실행시 문제가 발생할 수 있고, 반대로 서버가 컴파일 모드인데,
		// 클라이언트에서 정의를 파괴하지 않으면 메모리 낭비가 있을 수 있기 때문에
		// 런타임에서 파괴하는 것이다.
#ifdef __CLIENT		
		// 게임 마스터는 define을 파괴하면 안된다. 그렇게 하면 영자 명령 쓰는데 애로점이 생김.
		if( !g_pPlayer->IsAuthHigher( AUTH_GAMEMASTER ) )
			m_globalDef.Free();
#endif
	}
	else 
	{
		process = INTERPRITER;
		m_pProg = p;
	}
	do 
	{
		while(brace) 
		{ /* bypass code inside functions */
			return;
			if(*m_pProg=='{')	brace++; else
			if(*m_pProg=='}')	brace--;
			m_pProg++;
		}
		GetTkn();

		if(tok==DEFINE) 
			ExecDefine( bLocalDef );
		else
		if(tok==ENUM) 
		{
			eNum = 0; GetTkn(); // {
			do {
				GetTkn();
				char* pData = new char[10];
				if(LookupDefine(Token,(void*&)pData) == FALSE)
				{
					if( bLocalDef )
						m_localDef.SetAt(Token,pData);
					else
					{
						TRACE( "%s\n", Token );
						m_globalDef.SetAt(Token,pData);
					}
				}
				else
					TRACE("Enum 중복 에러 발생 : %s\n",Token);
				GetTkn(); //  , or =
				if(*token=='=') 
				{
					GetTkn(); // num
					eNum = atoi(token);
					GetTkn(); // ,
				}
				_itoa(eNum,pData,10);
				eNum++;
			} while(*token!='}');
			GetTkn(); // ;
		}
		else
		if(tok==INCLUDE) 
		{
			GetTkn(); //
			strcpy( fileName, token );
			CHAR* pOldProg = m_pProg;
			CHAR* pOldBuf = m_pBuf;
			int nOldSize = m_nProgSize;
			m_pProg = m_pBuf = 0;
			CScanner::Load( fileName );
			PreScan( TRUE ); 
			CScanner::Free();
			m_nProgSize = nOldSize;
			m_pProg = pOldProg;
			m_pBuf = pOldBuf; 
			tok = 0;
		}
		else 
		if(tok==VAR) 
		{ /* is global var */
			PutBack(); DeclGlobal();
		}
		else
		if(tokenType==IDENTIFIER) 
		{
			strcpy(temp,token);	GetTkn();
			if(*token=='(') 
			{ /* must be assume a function */
				funcTable[funcIndex].loc = m_pProg;
				strcpy(funcTable[funcIndex++].name,temp);
				while(*m_pProg!=')') 
					m_pProg++;	
				m_pProg++;
				/* m_pProg points to opening cyrly brace of function */
			}
			else PutBack();
		}
		else if(*token=='{') brace++;
	} while(tok!=FINISHED);
	m_pProg = p;
}
char *CScript::FindFunc(char *name)
{
	register int i;
	for(i = 0; i < funcIndex; i++)
		if(!strcmp(name, funcTable[i].name))
			return funcTable[i].loc;
	return NULL;
}
void CScript::DeclGlobal()
{
	GetToken(); /* get type */

	globalVars[gVarIndex].varType = tok;
	globalVars[gVarIndex].value = 0; /* init to 0 */

	do { /* process comma-separated list */
		GetToken(); /* get name */
		strcpy(globalVars[gVarIndex].varName, token);
		GetToken();
		gVarIndex++;
	} while(*token==',');
	if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
}
void CScript::DeclLocal()
{
	VarType i;

	GetToken(); /* get type */

	i.varType = tok;
	i.value = 0; /* init to 0 */

	do { /* process comma-separated list */
		GetToken(); /* get var name */
		strcpy(i.varName,token);
		LocalPush(i);
		GetToken();
	} while(*token==',');
	if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
}
void CScript::FindLabel()
{
	char block = 0;
	char* p = m_pProg;
	char temp[64];
	m_dwDef = 0;
	do 
	{
		GetToken();
		if( tokenType == IDENTIFIER ) 
		{
			strcpy( temp, token );
			GetToken();
			if( *token==':' ) 
			{
				strcpy( labelTable[ labeltos ].name, temp );
				labelTable[ labeltos ].loc = m_pProg;
				labelTable[ labeltos ].block = block;
				labeltos++;
				if( labeltos >= 45 ) 
				{
					printf("label overflow");
				}
			}
		}
		else
		if( tokenType == BLOCK ) 
		{ /* If block delimiter */
			if( *token == '{') block++;
			else block--;
			if( !block ) { m_pProg = p;	return; }
		}
	} while(tok != FINISHED);
	m_pProg = p;
	m_dwDef = 2;
}
void CScript::Call()
{
	char *loc;
	CHAR* temp;
	int lvartemp;
	FuncInfo fi;

	loc = FindFunc( token );     // find entry point of function
	if( loc == NULL )
		SntxErr( NULL, FUNC_UNDEF );   // function not defined
	else 
	{
		lvartemp = lvartos;    // save local var stack index
		GetArgs(); 			   // get function arguments
		temp = m_pProg; 		   // save return location
		fi.var    = lvartemp;
		fi.label  = labeltos;
		fi.answer = answerCnt;
		fi.block  = blocktos;
		retValue  = TRUE; // 펑션의 디폴트 리턴값은 1이다.
		FuncPush(fi);          // save local var stack index
		m_pProg      = loc; 	   // reset m_pProg to start of function
		GetParams(); 		   // load the functuon's parameters with the values of the arguments
		FindLabel();		   // find label and it's push label table
		blocktos  =
		retSign   = 0;
		InterpBlock(); 		   // interpret the function
		m_pProg      = temp; 	   // reset the program pointer
		fi        = FuncPop(); // reset the local var stack
		lvartemp  = fi.var;
		labeltos  = fi.label;
		answerCnt = fi.answer;
		lvartos   = fi.var;
		blocktos  = fi.block;
	}
}
void CScript::GetArgs()
{
	int value, count, temp[NUM_PARAMS];
	VarType i;
	count = 0;
	GetToken();
	if( *token != '(' ) 
		SntxErr( NULL, PAREN_EXPECTED);

	GetToken();
	if( *token == ')' ) 
		return;
	PutBack();

	/* process a comma-separated list of values */
	do 
	{
		Compute(&value);
		temp[count] = value; /* save temporarily */
		GetToken();
		count++;
	} while(*token==',');

	/* now, push on localVarStack in reverse order */
	for(count--; count >= 0; count--) 
	{
		i.value = temp[count];
		i.varType = ARG;
		LocalPush(i);
	}
}
void CScript::GetParams()
{
	if(!lvartos)
		GetToken();
	else 
	{
		VarType *p; int i = lvartos-1;
		do { /* process comma-separated list of parameters */
			GetToken();
			p = &localVarStack[i];
			if(*token!=')') {
				if(tok!=VAR) SntxErr( NULL, TYPE_EXPECTED);
				p->varType = tokenType;
				GetToken();

				/* link parameter name with argument already on local var stack */
				strcpy(p->varName,token);
				GetToken();
				i--;
			}
			else break;
		} while(*token==',');
	}
	if(*token!=')') SntxErr( NULL, PAREN_EXPECTED);
}
void CScript::FuncRet()
{
	int value;

	value = 0;
	/* get return valuem if any */
	Compute(&value);
	retValue = value;
}
void CScript::LocalPush(VarType i)
{
	if(lvartos > NUM_LOCAL_VARS)
		SntxErr( NULL, TOO_MANY_LVARS);
	localVarStack[lvartos] = i;
	lvartos++;
}
FuncInfo CScript::FuncPop()
{
	functos--;
	if(functos < 0) SntxErr( NULL, RET_NOCALL);
	return callStack[functos];
}
void CScript::FuncPush(FuncInfo fi)
{
	if(functos > NUM_FUNC)
		SntxErr( NULL, NEST_FUNC);
	callStack[functos] = fi;
	functos++;
}
void CScript::AssignVar(char *varName, int value)
{
	/* first, see if it's a loca variable */
	for(int i = callStack[functos-1].var; i < lvartos; i++) {
		if(!strcmp(localVarStack[i].varName,varName)) {
			localVarStack[i].value = value;	return;
		}
	}
	/* if not local, try global var table */
	for(int i = 0; i < gVarIndex; i++)
		if(!strcmp(globalVars[i].varName,varName)) {
			globalVars[i].value = value; return;
		}
	SntxErr( NULL, NOT_VAR); /* variable not found */
}
int* CScript::GetVarPtr(char *varName)
{
	/* first, see if it's a loca variable */
	for(int i = callStack[functos-1].var; i < lvartos; i++) 
	{
		if(!strcmp(localVarStack[i].varName,varName)) 
		{
			return &localVarStack[i].value;
		}
	}
	/* if not local, try global var table */
	for(int i = 0; i < gVarIndex; i++)
	{
		if(!strcmp(globalVars[i].varName,varName)) 
		{
			return &globalVars[i].value;
		}
	}
	SntxErr( NULL, NOT_VAR); /* variable not found */
	return NULL;
}

int CScript::FindVar(char *s)
{
	/* first, see if it's a local variable */
	for(int i = callStack[functos-1].var; i < lvartos; i++)
		if(!strcmp(localVarStack[i].varName,s))
			return localVarStack[i].value;
	/* otherwise, try global vars */
	for(int i = 0; i < gVarIndex; i++)
		if(!strcmp(globalVars[i].varName,s))
			return globalVars[i].value;
	SntxErr( NULL, NOT_VAR); /* vaiable not found */
	return 0;
}
int CScript::IsVar(char *s)
{
	/* first, see if it's a local variable */
	for(int i = callStack[functos-1].var; i < lvartos; i++)
		if(!strcmp(localVarStack[i].varName,s))
			return 1;
	/* otherwise, try global bars */
	for(int i = 0; i < gVarIndex; i++)
		if(!strcmp(globalVars[i].varName,s))
			return 1;
	return 0;
}
/* Find end of block
 *
 *  {     }!
 *  {  ;  }!
 *        ;!
 *  ( ; ) ;!
 *  ( ) { }!
 *  { (;) }!
 *  ! is end
 */
void CScript::FindEob(void)
{
	int brace = 0;
	for(;;) {
		GetToken();
		if(*token==';'&&!brace) break;
		else if(*token=='{') brace++;
		else if(*token=='}') { if(!(--brace)) break; }
		else if(*token=='(') {
			int bra = 1;
			while(bra) {
				GetToken();
				if(*token=='(') bra++;
				if(*token==')') bra--;
			}
		}
	}
}
void CScript::ExecDefine( BOOL bLocalDef )
{
	GetTkn();
	char* pData2;
	if(LookupDefine(Token,(void*&)pData2) == TRUE)
	{
		TRACE("Define 중복 에러 발생 : %s\n",Token);
		return;
	}
	char* tmp = token; 
	int c = 0;
	CHAR* pProg = (CHAR*) m_pProg;
	while( *pProg !='\r' && *pProg != '\0' ) 
	{
		*tmp++ = *pProg++; 
		c++;
	}
	*tmp = '\0';
	char* pData = new char[ c + 1 ];
	strcpy( pData, token );
	if( bLocalDef )
		m_localDef.SetAt( Token, pData );
	else
	{
		TRACE( "%s\n", Token );
		m_globalDef.SetAt( Token, pData );
	}
}
void CScript::ExecIf()
{
	int cond; 
	CHAR* begin;

	Compute(&cond); // get left expression

	if(cond) 
	{ // is true so process target of IF
		begin = m_pProg;
		switch(InterpBlock()) 
		{
			case 2:	m_pProg = begin; FindEob();
			case 1: case 3: return;
		}
	}
	else 
	{    // otherwise skip around IF block and process the ELSE, if present
		GetToken();
		if(*token!='{') 
		{
			int flag = 0, iff = 0;
			for(;;) 
			{
				if(flag && (!iff || (iff && tok!=ELSE)))
					break;
				if(tok==IF) {
					iff++; flag = 0;
				}
				else
				if(tok==ELSE) {
					iff--; flag = 0;
				}
				else {
					PutBack();
					FindEob();
					flag = 1;
				}
				GetToken();
			}
		}
		else 
		{
			PutBack();
			FindEob(); // find start of next line
			GetToken();
		}
		if( tok != ELSE ) 
		{
			PutBack(); // restore token if no ELSE if present
			return;
		}
		begin = m_pProg;
		switch( InterpBlock() ) 
		{
			case 2:	m_pProg = begin; FindEob();
			case 1: case 3: return;
		}
	}
}
void CScript::ExecWhile()
{
	int cond;
	PutBack();
	CHAR* temp = m_pProg; // save location of top of while loop
	GetToken();
	Compute(&cond); // check the conditional expression
	if(cond) 
	{
		CHAR* begin = m_pProg;
		switch( InterpBlock() ) 
		{
			case 2: m_pProg = begin; FindEob();
			case 1:	case 3: return;
		}
	}
	else 
	{ // otherwise, skip around loop
		FindEob();
		return;
	}
	m_pProg = temp; // loop back to top
}
void CScript::ExecDo()
{
	int cond;	
	PutBack();
	CHAR* temp = m_pProg; // save location of top of do loop
	GetToken();  // get start of loop
	CHAR* begin = m_pProg;
	switch(InterpBlock()) 
	{
		case 2:	m_pProg = begin; FindEob(); FindEob(); // bypassed block and while
		case 1:	case 3: return;
	}
	GetToken();
	if(tok!=WHILE) 
		SntxErr( NULL, WHILE_EXPECTED);
	Compute(&cond);       // check the loop condition
	if( cond ) 
		m_pProg = temp; // if true loop; otherwise, cotinue on
}
void CScript::ExecFor()
{
	int cond;
	char *temp, *temp2;
	int brace;

	GetToken();
	Compute(&cond); /* initialization expression */
	if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
	m_pProg++; /* get past the ; */
	temp = m_pProg;
	for(;;) {
		cond = 1; // 다음 Compute에서 비교대상이 없으면 (;;) cond = 1을 유지하여 무한루프를 돌게 한다. 
		Compute(&cond); /* check the condition */
		if(*token!=';') SntxErr( NULL, SEMI_EXPECTED);
		m_pProg++; /* get past the ; */
		temp2 = m_pProg;

		/* find the start of the for block */
		brace = 1;
		while(brace) {
			GetToken();
			if(*token=='(') brace++;
			if(*token==')') brace--;
		}
		if(cond) {
			char *begin = m_pProg;
			switch(InterpBlock()) {
				case 2: m_pProg = begin; FindEob();
				case 1:	case 3: return;
			}
		}
		else  { /* otherwise, skip around loop */
			FindEob();
			return;
		}
		m_pProg = temp2;
		Compute(&cond); /* do the increment */
		m_pProg = temp; /* loop back to top */
	}
}
void CScript::ExecSwitch()
{
	int cond,cond2,block = 0;
	char *def   = 0;
	char *begin = m_pProg;

	Compute(&cond);

	for(;;) {
		GetToken();
		if(*token=='{') {
			if(block) {
				PutBack();
				FindEob();
			}
			else block = 1;
		}
		else
		if(*token=='}') {
			if(def) {
				m_pProg = def;
				if(InterpBlock(1) == 2) {
					m_pProg = begin; FindEob();
				}
			}
			break;
		}
		else
		if(!def) {
			if(tok == CASE) {
				Compute(&cond2);
				if(cond2==cond) {
					GetToken(); // bypass ':'
					def = m_pProg;
				}
			}
			else
			if(tok == DEFAULT) {
				GetToken(); // bypass ':'
				def = m_pProg;
			}
		}
	}
}
void CScript::Free()
{
	CScanner::Free(); 
	process = INTERPRITER;
	m_pOldProg       = 0;
	functos       = 0; // index to top of function call stack
	funcIndex     = 0; // index to into function table
	gVarIndex     = 0; // index into global variable table
	lvartos       = 0; // index into local variable stack
	labeltos      = 0; // index into local label table
	retValue      = 0; // function return value
	blocktos      = 0; //
	answerCnt     = 0;
	labelIndex    =-1;
	retSign	      = 0;
#ifndef __V050322_CACHEQUEST
	memset(m_globalArray,0,MAX_GLOBAL_ARRAY*sizeof(int));
#endif
	//CScanner::Free(); //globalVars = 0;
}
BOOL CScript::Load( LPCTSTR lpszFileName, BOOL bMultiByte, int nProcess)
{
	if( !CScanner::Load( lpszFileName, bMultiByte ) ) 
		return 0;
	if(nProcess == INTERPRITER)
	{
		process = nProcess;
		return TRUE;
	}
	Compile();
	return TRUE;
}
void CScript::FreeGlobalDefine()
{
	m_globalDef.Free();
	m_localDef.Free();
} 
BOOL CScript::LookupDefine( LPCTSTR lpszString, void*& rValue )
{
	if( m_globalDef.Lookup( lpszString, (void*&)rValue ) == TRUE )	
		return TRUE;
	return m_localDef.Lookup( lpszString, (void*&)rValue );
}
//
// 그 Define에 스트링을 돌려준다.
//
LPSTR CScript::GetDefineStr(LPCTSTR lpId)
{
	char* pData = NULL;
	LookupDefine(lpId,(void*&)pData);
	return pData;
}
BOOL CScript::AddDefine( LPCTSTR lpszDefine, int nData )
{
	CString string;
	string.Format( _T( "%d" ), nData );
	TCHAR* pData = new TCHAR[ string.GetLength() + sizeof( TCHAR ) ];
	_tcscpy( pData, string );
	TRACE( "%s\n", lpszDefine );
	m_globalDef.SetAt( lpszDefine, (void*)pData );
	return TRUE;
}
BOOL CScript::AddDefine( LPCTSTR lpszDefine, LPCTSTR lpszData )
{
	TCHAR* pData = new TCHAR[ _tcslen( lpszData ) + sizeof( TCHAR ) ];
	_tcscpy( pData, lpszData );
	TRACE( "%s\n", lpszDefine );
	m_globalDef.SetAt( lpszDefine, (void*)pData );
	return TRUE;
}
BOOL CScript::RemoveDefine( LPCTSTR lpszDefine )
{
	LPSTR lpszData;
	if( LookupDefine( lpszDefine, (void*&)lpszData ) )
	{
		SAFE_DELETE( lpszData );
		m_globalDef.RemoveKey( lpszDefine );
		return TRUE;
	}
	return FALSE;
}

//
// remark 
//   lpId라는 define의 내용이 숫자라면 숫자를 돌려준다.
// 
// return 
//   그 Define에 Number를 돌려준다. 다른 경우 -1
//  
int CScript::GetDefineNum(LPCTSTR lpId)
{
	char* pData = NULL;
	if(LookupDefine(lpId,(void*&)pData) == FALSE)
		return -1;
	return atoi(pData);
}
//
// lpStrDef와 일치하는 문자열이 있는 define을 pStrArray에 담아준다.
// (반드시 일치하지 않아도 된다.)
//
void CScript::GetFindDefToArray(LPCTSTR lpStrDef,CStringArray* pStrArray)
{
	CString string;
	char* pData;
	POSITION pos 
		= m_globalDef.GetStartPosition();
	while(pos)
	{ 
		m_globalDef.GetNextAssoc(pos,string,(void*&)pData);
		if(string.Find(lpStrDef) != -1)
			pStrArray->Add(string);
	}
}
//
// lpStrDef와 일치하는 문자열이 있는 define과 내용을 pStrArray에 담아준다.
// (반드시 일치하지 않아도 된다.)
//
/*
void CScript::GetFindDefDataToArray( LPCTSTR lpStrDef, CPtrArray* pPtrArray )
{
	CString string;
	LPDEFINEDATA lpDefine;
	TCHAR* pData;
	POSITION pos 
		= m_globalDef.GetStartPosition();
	while(pos)
	{ 
		m_globalDef.GetNextAssoc( pos, string, (void*&)pData );
		if( string.Find( lpStrDef ) != -1 )
		{
			lpDefine = new DEFINEDATA;
			lpDefine->strDefine = string;
			lpDefine->pData = pData;
			pPtrArray->Add( lpDefine );
		}
	}
}
*/
//
// XX_로 시작하는 것을 찾아준다.
//
void CScript::GetFindIdToArray(LPCTSTR lpStrDef,CStringArray* pStrArray)
{
	int i;
	CString string;
	char* pData;
	POSITION pos 
		= m_globalDef.GetStartPosition();
	int nLength = strlen(lpStrDef);//strDef.GetLength();
	while(pos)
	{ 
		m_globalDef.GetNextAssoc(pos,string,(void*&)pData);
		for(i = 0; i < nLength; i++)
			if(lpStrDef[i] != string[i])
				break;
		if(i == nLength)
			pStrArray->Add(string);
	}
}
//
// XX_로 시작하는 것중에 그것의 숫자가 nNum와 같으면 그 define을 돌려준다.
//
CString CScript::GetFindIdStr(LPCTSTR lpStrDef,int nNum)
{
	int i;
	CString string;
	char* pData;
	POSITION pos 
		= m_globalDef.GetStartPosition();
	int nLength = strlen(lpStrDef);//.GetLength();
	while(pos)
	{ 
		m_globalDef.GetNextAssoc(pos,string,(void*&)pData);
		for(i = 0; i < nLength; i++)
			if(lpStrDef[i] != string[i])
				break;
		if(i == nLength && GetDefineNum(string) == nNum)
			return string;
	}
	TCHAR szNum[64];
	_itot( nNum, szNum, 10 );
	string = szNum;
	return string; 
}
CMapDefineToPtr::~CMapDefineToPtr()
{
	Free();
}
void CMapDefineToPtr::Free()
{
	CString string;
	char* pData;
	POSITION pos 
		= GetStartPosition();
	while(pos)
	{ //CMapStringToPtr
		GetNextAssoc(pos,string,(void*&)pData);
		delete pData;
	}
	RemoveAll();
}

/*
int CMapDefineToPtr::GetCount() 
{
}
BOOL CMapDefineToPtr::IsEmpty() const
{
}

BOOL CMapDefineToPtr::Lookup(LPCTSTR key, void*& rValue) const
{
}

BOOL CMapDefineToPtr::LookupKey(LPCTSTR key, LPCTSTR& rKey) const
{
}

void*& CMapDefineToPtr::operator[](LPCTSTR key)
{
}

void CMapDefineToPtr::SetAt(LPCTSTR key, void* newValue)
{
}

BOOL CMapDefineToPtr::RemoveKey(LPCTSTR key)
{
}

void CMapDefineToPtr::RemoveAll()
{
}

POSITION CMapDefineToPtr::GetStartPosition() const
{
}

void CMapDefineToPtr::GetNextAssoc(POSITION& rNextPosition, CString& rKey, void*& rValue) const
{
}
void CMapDefineToPtr::AddDefineMap(LPCTSTR lpDefineFileName,BOOL bGlobalDef)
{
	LPDEFINETYPE lpDefineType = new DEFINETYPE;
	lpDefineType->m_pMapDefine = new CMapStringToPtr;
	SetAt(lpDefineFileName,lpDefineType);
	m_pActMapDefine = lpDefineType->m_pMapDefine;
}
void CMapDefineToPtr::DestroyDefineMap(LPCTSTR lpDefineFileName)
{
	LPDEFINETYPE lpDefineType;
	Lookup(lpDefineFileName,(void*&)lpDefineType);

	delete lpDefineType;
}
*/
CMapDefineToPtr CScript::m_globalDef;
CMapDefineToPtr CScript::m_localDef;
#ifdef __USE_STR_TBL0407
map<string, string>	CScript::m_mapString;
#endif	// __USE_STR_TBL0407
/////////////////
//
//
//
//
//
//

/*
BYTE CScript::Run(LPCTSTR lpszFileName)
{
	TRY
		if(!Load(lpszFileName)) 
			return 0;
		m_pProg = FindFunc("main"); m_pProg--;
		strcpy(token,"main");
		Call();	
		Free(); 
		return 1;
	AND_CATCH_ALL(e)
		TRACE("Throw Exception\n");
	END_CATCH_ALL
	Free(); 
	return 0;
}
*/
BOOL CScript::Run()
{
#ifndef __V050322_CACHEQUEST
  m_nRunScript++;
	m_bQuitDialog = FALSE;
	m_bPressDialog = FALSE;	
#endif
	m_dwDef = 2;
	m_pProg        = m_pBuf;
	m_pOldProg     = 0;
	functos     = 0; 
	funcIndex   = 0; 
	gVarIndex   = 0; 
	lvartos     = 0; 
	labeltos    = 0; 
	retValue    = 0; 
	blocktos    = 0; 
	answerCnt   = 0;
	labelIndex  =-1;
	retSign	    = 0;
	//TRY
		PreScan( TRUE );
		m_pProg = FindFunc("main");
//		if(m_pProg == NULL)
//;//			AfxThrowUserException();
		m_pProg--;
		strcpy(token,"main");
		Call();	
		
#ifdef __V050322_CACHEQUEST
		m_anInternalVal[0] = retValue; 
#else		
		m_globalArray[0] = retValue; 
		m_nRunScript--;
		m_bQuitDialog = FALSE;
		m_bPressDialog = FALSE;	
#endif
		return TRUE;
	//AND_CATCH_ALL(e)
		TRACE("Throw Exception\n");
//	END_CATCH_ALL
#ifndef __V050322_CACHEQUEST
	m_nRunScript--;
	m_bQuitDialog = FALSE;
	m_bPressDialog = FALSE;	
#endif
	m_localDef.RemoveAll();
	return FALSE;
}
/*
BOOL CScript::operator () (LPCTSTR lpszFileName)
{
	return Run(lpszFileName);
}
*/
BOOL CScript::operator () ()
{
	return Run();
}
/////////////////

int CScript::IsFunc(char *str)
{
	for(register int i = 0; interFunc[i].p; i++)
		if(!strcmp(interFunc[i].name,str))
			return i;
	return -1; /* unknown command */
}
void CScript::Write(LPSTR* cplProg,char* str)
{
	/*
	BYTE beg[256], *s; s = beg; strcpy(beg,str);
	do {
		*s = ~(*s);
		*s = (*s << 4) | (*s >> 4);
	} while(*(++s));
	*/
	while(*str) 
		*((*cplProg)++) = *(str++);
}
void CScript::Compile()
{
	int r = 0; 
	char str[256]; 
	LPSTR cplProg  = new CHAR[m_nProgSize];
	LPSTR pBakProg = cplProg;
	Write(&cplProg,"# ");
	SetInterpriterMode();
	do {
		GetToken();
		switch(tokenType) 
		{
			case STRING:
				Write(&cplProg,"\"");
				Write(&cplProg,token);
				Write(&cplProg,"\"");
				break;
			case IDENTIFIER:
				// 인터널 펑션만 컴파일한다.
				if((r=IsFunc(token))!=-1) 
				{
					str[0] = '$';	_itoa(r,&str[1],10);
					Write(&cplProg,str);
				}
				else Write(&cplProg,token);
				break;
			case KEYWORD:
				switch(cmd) {
					case DEFINE: // define
					case INCLUDE: // include
						while(*m_pProg!='\r'&&*m_pProg!='\0') m_pProg++;
						break;
					case ENUM: // enum
						GetTkn(); // {
						do 
						{
							GetTkn();
						} while(*token!='}');
						GetTkn(); // ;
						break;
					default:
						str[0] = '@';	_itoa(cmd,&str[1],10);
						Write(&cplProg,str);
						break;
				}
				switch(cmd) 
				{
				case ELSE: case DO: case VAR: case RETURN: case GOTO: case CASE: 
					Write(&cplProg," "); break;
				}
				break;
			default:
				Write(&cplProg,token);
				break;
		}
	} while(tok!=FINISHED);
	CScanner::Free();	*cplProg = 0; cplProg++;
	m_pProg = new CHAR[(int)(cplProg-pBakProg)];
	memcpy(m_pProg,pBakProg,(int)(cplProg-pBakProg));
	delete pBakProg;
	m_bMemFlag = 0; m_pBuf = m_pProg; 
	SetCompilerMode();
}
/*
void CScript::SetGlobal(CUIntArray &globalArray)
{
	if(globalArray.GetSize() >= 4)
		m_globalArray.SetSize(globalArray.GetSize());
	else
		globalArray.SetSize(4);
	for(int i = 0; i < globalArray.GetSize(); i++)
		m_globalArray[i] = globalArray[i];
}
*/
#ifdef __V050322_CACHEQUEST
void CScript::SetIValue(int nVal,...) 
{
	va_list args;
	va_start( args, nVal );
	int i = 0;
	while( nVal != -1 )
	{
		m_anInternalVal[ i++ ] = nVal;
		nVal = va_arg( args, int );
	}
	va_end( args );
}
#else // __V050322_CACHEQUEST
//{{AFX
void CScript::SetGlobal(int nVal,...) 
{
	va_list args;
	va_start(args,nVal);
	int i = 0;
  while(nVal != -1 )
  {
    m_globalArray[i++] = nVal;
		nVal = va_arg(args,int);
  }
	va_end(args);
}
//}}AFX
#endif // __V050322_CACHEQUEST

#if !defined( __CLIENT ) && !defined( __WORLDSERVER ) ///&& !defined( __DBSERVER )
InterFuncType interFunc[] = { 0, 0, 0 };
#endif
