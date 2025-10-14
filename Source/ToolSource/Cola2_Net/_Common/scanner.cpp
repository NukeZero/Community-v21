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
#include "vutil.h"
#include "xutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/***************
 * 토큰 스트림 *
 ***************/

//IMPLEMENT_SERIAL(CScanner,CObject,0)

#ifdef _MULTI_LANGUAGE_

WORD g_codePage = 949;

/*----------------------------------------------------------------------------*/ 
/*
namespace THAI
{
	enum CLASS {	
		CTRL= 0,
		 NON= 1,
		CONS= 2,
		  LV= 3,
		 FV1= 4,
		 FV2= 5,
		 FV3= 6,
		 BV1= 7,
		 BV2= 8,
		  BD= 9,
		TONE=10,
		 AD1=11,
		 AD2=12,
		 AD3=13,
		 AV1=14,
		 AV2=15,
		 AV3=16,
	};

	CLASS	CLASS_TABLE[256] = {
		CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
		CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
		 NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
		 NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
		 NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
		 NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
		 NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
		 NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
		CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
		CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
		 NON, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
		CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
		CONS, CONS, CONS, CONS,  FV3, CONS,  FV3, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS,  NON, 
		 FV1,  AV2,  FV1,  FV1,  AV1,  AV3,  AV2,  AV3,  BV1,  BV2,   BD,  NON,  NON,  NON,  NON,  NON, 
		  LV,   LV,   LV,   LV,   LV,  FV2,  NON,  AD2, TONE, TONE, TONE, TONE,  AD1,  AD1,  AD3,  NON, 
		 NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
	};

	namespace INPUT
	{
		enum TYPE { A, C, S, R, X };

		bool COMPOSIBLE[3][5] = {
			{ true, true, true, true, true },
			{ true, true, true,false, true },
			{ true, true,false,false, true },
		};

		TYPE STATE_CHECK[17][17] = {
			{ X,A,A,A,A,A,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,S,A,C,C,C,C,C,C,C,C,C,C },
			{ X,S,A,S,S,S,S,R,R,R,R,R,R,R,R,R,R },
			{ X,S,A,S,A,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,A,S,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,S,A,R,R,R,C,C,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,A,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,C,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,R,C,R,R,R,R },
		};
	}

	namespace OUTPUT
	{
		enum TYPE { N, C, X };

		TYPE STATE_CHECK[17][17] = {
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,C,C,C,C,C,C,C,C,C,C },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,C,C,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,C,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,N,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,C,C,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,C,N,N,N,N,N,N },
			{ N,N,N,N,N,N,X,N,N,N,C,N,C,N,N,N,N },
		};
	}
}
*/

namespace THAI
{
	enum CLASS {	
		CTRL= 0,
			NON= 1,
			CONS= 2,
			LV= 3,
			FV1= 4,
			FV2= 5,
			FV3= 6,
			BV1= 7,
			BV2= 8,
			BD= 9,
			TONE=10,
			AD1=11,
			AD2=12,
			AD3=13,
			AV1=14,
			AV2=15,
			AV3=16,
	};
	
	CLASS	CLASS_TABLE[256] = {
		CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
			CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, CTRL, 
			NON, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
			CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS, 
			CONS, CONS, CONS, CONS,  FV3, CONS,  FV3, CONS, CONS, CONS, CONS, CONS, CONS, CONS, CONS,  NON, 
			FV1,  AV2,  FV1,  FV1,  AV1,  AV3,  AV2,  AV3,  BV1,  BV2,   BD,  NON,  NON,  NON,  NON,  NON, 
			LV,   LV,   LV,   LV,   LV,  FV2,  NON,  AD2, TONE, TONE, TONE, TONE,  AD1,  AD1,  AD3,  NON, 
			NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON,  NON, CTRL, 
	};
	
	namespace INPUT
	{
		enum TYPE { A, C, S, R, X };
		
		bool COMPOSIBLE[3][5] = {
			{ true, true, true, true, true },
			{ true, true, true,false, true },
			{ true, true,false,false, true },
		};

		TYPE STATE_CHECK[17][17] = {
		//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 
			{ X,A,A,A,A,A,A,R,R,R,R,R,R,R,R,R,R }, // 0 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 1 
			{ X,A,A,A,A,S,A,C,C,C,C,C,C,C,C,C,C }, // 2 
			{ X,S,A,S,S,S,S,R,R,R,R,R,R,R,R,R,R }, // 3 
			{ X,S,A,S,A,S,A,R,R,R,R,R,R,R,R,R,R }, // 4 
			{ X,A,A,A,A,S,A,R,R,R,R,R,R,R,R,R,R }, // 5 
			{ X,A,A,A,S,A,S,R,R,R,R,R,R,R,R,R,R }, // 6 
			{ X,A,A,A,A,S,A,R,R,R,C,C,R,R,R,R,R }, // 7 
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R }, // 8 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 9 
			{ X,A,A,A,A,A,A,R,R,R,R,R,R,R,R,R,R }, // 0 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 1 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 2 
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R }, // 3 
			{ X,A,A,A,S,S,A,R,R,R,C,C,R,R,R,R,R }, // 4 
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R }, // 5 
			{ X,A,A,A,S,S,A,R,R,R,C,R,C,R,R,R,R }, // 6 
		};
		/*
		TYPE STATE_CHECK[17][17] = {
			{ X,A,A,A,R,R,R,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,R,R,R,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,S,A,C,C,C,C,C,C,C,C,C,C },
			{ X,S,A,S,S,S,S,R,R,R,R,R,R,R,R,R,R },
			{ X,S,A,S,A,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,A,S,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,S,A,R,R,R,C,C,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,A,A,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,R,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,C,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,R,R,R,R,R,R },
			{ X,A,A,A,S,S,A,R,R,R,C,R,C,R,R,R,R },
		};
		*/
	}
	
	namespace OUTPUT
	{
		enum TYPE { N, C, X };
		/*
		TYPE STATE_CHECK[17][17] = {
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,C,C,C,C,C,C,C,C,C,C,C,C,C },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,C,C,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,C,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,C,C,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,C,N,N,N,N,N,N },
			{ X,N,N,N,N,N,N,N,N,N,C,N,C,N,N,N,N },
		};
		*/
		//    0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 
		TYPE STATE_CHECK[17][17] = {
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 0 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 1 
			{ X,N,N,N,N,N,N,C,C,C,C,C,C,C,C,C,C }, // 2 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 3 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 4 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 5 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 6 
			{ X,N,N,N,N,N,N,N,N,N,C,C,N,N,N,N,N }, // 7 
			{ X,N,N,N,N,N,N,N,N,N,C,N,N,N,N,N,N }, // 8 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 9 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 0 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 1 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 2 
			{ X,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N,N }, // 3 
			{ X,N,N,N,N,N,N,N,N,N,C,C,N,N,N,N,N }, // 4 
			{ X,N,N,N,N,N,N,N,N,N,C,N,N,N,N,N,N }, // 5 
			{ X,N,N,N,N,N,N,N,N,N,C,N,C,N,N,N,N }, // 6 
		};
	}
	
}

bool	IsComposibleTh(BYTE prev, BYTE curr, int mode)
{
	using namespace THAI;
	using namespace INPUT;

	if(mode > 2) {
		return false;
	} else {
		return COMPOSIBLE[mode][STATE_CHECK[CLASS_TABLE[prev]][CLASS_TABLE[curr]]];
	}
}

// Unicode of thai : 0x0E00~0x0E7F (Direct Mapping)
// Thai Character -> Unicode    (char+0x0E00)-0xA0 =    char+0x0D60
// Unicode -> Thai Character (wchar_t-0x0E00)+0xA0 = wchar_t-0x0D60

const char* CharNextTh(const char* lpsz)
{
	using namespace THAI;
	using namespace OUTPUT;

	const BYTE* stream = (const BYTE*)(lpsz);

	while(STATE_CHECK
		[ 
			CLASS_TABLE
			[ 
				stream[ 0 ] 
			] 
		]
		[ 
			CLASS_TABLE
			[ 
				stream[ 1 ] 
			] 
		] == C) ++stream;

	return (const char*)(stream+1);
}

const char*	CharNextEx(const char* strText, WORD codePage)
{
	if(codePage == 874)
	{
		return CharNextTh(strText);
	}
	else
	{
		return CharNextExA(g_codePage, strText, 0);
	}
}

#endif //_MULTI_LANGUAGE_



/*
void CScanner::Serialize(CArchive& ar)
{
	if(ar.IsStoring())
		ar.Write(m_pBuf,m_nProgSize);
	else
	{
		m_nProgSize = ar.GetFile()->GetLength();
		m_pProg = m_pBuf = new char[m_nProgSize+1];
		ar.Read(m_pBuf,m_nProgSize);
		*(m_pProg+m_nProgSize) = '\0';
	}
}
*/
CScanner::CScanner( BOOL bComma )
{
	m_bComma = bComma;
	m_pProg = m_pBuf = NULL;
	m_bMemFlag = 1;
	dwValue	= 0;
	m_lpMark = NULL;
	m_dwDef = 0;
	token = m_mszToken;
	m_bWideChar = FALSE;
	m_bErrorCheck = TRUE;
	m_szFileName[ 0 ] = 0;
}
CScanner::CScanner( LPVOID p, BOOL bComma )
{
	m_bComma = bComma;
	m_pProg = m_pBuf = (CHAR*)p;
	m_bMemFlag = 1;
	dwValue	= 0;
	m_dwDef = 0;
	token = m_mszToken;
	m_bWideChar = FALSE;
	m_bErrorCheck = TRUE;
	m_szFileName[ 0 ] = 0;
}
CScanner::~CScanner()
{
	Free();
}
void CScanner::Free()
{
	if(!m_bMemFlag && m_pBuf) 
		delete m_pBuf;
	m_pProg = m_pBuf = 0;
}
BOOL CScanner::Read( CFileIO* pFile, BOOL bMultiByte  )
{
	m_bMemFlag = 0;
	char* pProg;
	m_nProgSize = pFile->GetLength();
	pProg = new char[ m_nProgSize + 2 ];
	m_pProg = m_pBuf = pProg;
	if( !m_pBuf ) return 0;
	pFile->Read( m_pBuf,m_nProgSize );
	*(pProg + m_nProgSize ) = '\0';
	*(pProg + m_nProgSize + 1 ) = '\0';
	// 0xfffe의 2바이트를 빼고 널(0x0000)을 2바이트를 추가해서 결국 m_nProgSize는 변화 없음.
	// 텍스트가 유니코드일 경우 멀티바이트로 변경 
	/*
	int lpi;
	BOOL b = IsTextUnicode( (LPVOID)m_pProg, nLength, NULL );//&lpi );
	CString strUnicode;
	CString strProg = m_pProg;
	strProg = strProg.Left( 250 );
	strUnicode.Format( "%s - %s (%d) Unicode Flag %x \n", lpszFileName, strProg, b, lpi );
	AfxMessageBox( strUnicode );
	if( lpi == IS_TEXT_UNICODE_ASCII16 )
	*/
	//if( IsTextUnicode( (LPVOID)m_pProg, nLength, NULL ) ) //NT only :-<
	if( (BYTE)*(pProg + 0 ) == 0xff && (BYTE)*(pProg + 1 ) == 0xfe ) // is unicode ?
	{
		m_bWideChar = TRUE;
		if( bMultiByte ) // is unicode ?
		{
			CHAR* lpMultiByte = new CHAR[ m_nProgSize + 2 ];
#if !defined(_MULTI_LANGUAGE_)
			int nResult = WideCharToMultiByte( CP_ACP, 0, (LPCWSTR)(pProg + 2), -1, lpMultiByte, 
#else //_MULTI_LANGUAGE_
			int nResult = WideCharToMultiByte( g_codePage, 0, (LPCWSTR)(pProg + 2), -1, lpMultiByte, 
#endif //_MULTI_LANGUAGE_
				(m_nProgSize + 2) * sizeof(CHAR), NULL, NULL );
			//cchWideChar
			lpMultiByte[ nResult ] = '\0';
			memcpy( pProg, lpMultiByte, nResult );
			delete lpMultiByte;
			m_bWideChar = FALSE;
		}
		else
		{
			memcpy( pProg, pProg + 2, m_nProgSize );
			//CHAR* lpMultiByte = new CHAR[ m_nProgSize + 1 ];
			//lpMultiByte[ nResult ] = '\0';
			//memcpy( pProg, lpMultiByte, nResult );
			//delete lpMultiByte;
			//m_bWideChar = FALSE;
		}
	}
	//m_pProg = m_pBuf;
	/*
#if !defined( __RESOURCE_TOOL ) && defined( __CLIENT )
	if( file.IsEncryption() )
	{
		do {
			*m_pProg = (*m_pProg << 4) | ((BYTE)*m_pProg >> 4);
			*m_pProg = ~(*m_pProg);
		} while(*(++m_pProg));
		m_pProg = m_pBuf;
	}
#endif
	*/
	return 1;
}
BOOL CScanner::Load( LPCTSTR lpszFileName, BOOL bMultiByte )
{
#ifdef __COLA
	return Load_FileIO( lpszFileName );
#endif
	CResFile file; 
	CString fileName = lpszFileName;
	fileName.MakeLower();
	if( !file.Open(lpszFileName, "rb" ) ) 
		return 0;
	strcpy( m_szFileName, lpszFileName );
	return Read( &file, bMultiByte );
}
#include <comdef.h>
BOOL CScanner::Load_FileIO( LPCTSTR lpszFileName, BOOL bMultiByte )
{
	CFileIO file;
	CString fileName = lpszFileName;
	fileName.MakeLower();
	if( !file.Open(lpszFileName, "rb" ) ) 
		return 0;
	strcpy( m_szFileName, lpszFileName );
	return Read( &file, bMultiByte );
}
void CScanner::SetProg( LPVOID pProg )
{
	m_bMemFlag = 1; m_pProg = m_pBuf = (CHAR*)pProg;
}
int CScanner::GetLineNum( LPVOID lpProg )
{
	int nLine = 0;
	if( lpProg == NULL )
		lpProg = m_pProg;
	
	if( m_bWideChar == FALSE )
	{
		CHAR* pTemp = m_pBuf;
		int nLine = 0;
		while( lpProg != pTemp )
		{
			if( *pTemp == '\r' )
				nLine++;
			pTemp++;
		}
		return nLine;
	}
	else
	{
		wchar_t* pTemp = (wchar_t*)m_pBuf;
		int nLine = 0;
		while( lpProg != pTemp )
		{
			if( *pTemp == '\r' )
				nLine++;
			pTemp++;
		}
		return nLine;
	}
	return nLine;
}
void CScanner::GetLastFull() // 현재부터 끝까지 한번에 읽는다.
{
	if( m_bWideChar )
	{
		wchar_t* pProg = (wchar_t*)m_pProg;
		// 앞쪽 white space를 스킵
		while( iswhite( *pProg ) && *pProg && *pProg != '\r' ) 
			pProg++;
		
		wchar_t* pTemp = m_wszToken; 
		while( *pProg && *pProg!='\r' )
		{
			*pTemp++ = *pProg++;
		}
		// 뒤쪽 white space를 스킵
		if( pTemp != m_wszToken )
		{
			pTemp--;
			while( iswhite( *pTemp ) && *pTemp ) 
				pTemp--;
			pTemp++;
			//if( isdigit( token[0] ) )
			//	tokenType = NUMBER;
			//if(token[0]=='0' && token[1]=='x') 
			//	tokenType = HEX;
		}
		m_pProg = (char*)pProg;
		*pTemp = '\0';

#if !defined(_MULTI_LANGUAGE_)
		int nLength = wcslen( m_wszToken ); 
		_wcstombsz( m_mszToken, m_wszToken, ( nLength * 2 ) + 1 );
#else //_MULTI_LANGUAGE_
		WideCharToMultiByte(g_codePage, 0, m_wszToken, -1, m_mszToken, sizeof(m_mszToken), NULL, NULL);
#endif //_MULTI_LANGUAGE_
		Token = m_wszToken;
	}
	else
	{
		// 앞쪽 white space를 스킵
		while( iswhite( *m_pProg ) && *m_pProg && *m_pProg != '\r' ) 
			m_pProg++;
		
		char* pTemp = token; 
		while( *m_pProg && *m_pProg!='\r' )
		{
#if !defined(_MULTI_LANGUAGE_)
#ifdef __CHARNEXT
			CopyChar( &m_pProg, &pTemp );
#else
			if( IsDBCSLeadByte( *m_pProg ) )
				*pTemp++ = *m_pProg++;
			*pTemp++ = *m_pProg++;
#endif
#else //_MULTI_LANGUAGE_
			int count = CopyChar( m_pProg, pTemp );
			m_pProg += count;
			pTemp += count;
#endif //_MULTI_LANGUAGE_
		}
		// 뒤쪽 white space를 스킵
		if( pTemp != token )
		{
			pTemp--;
			while( iswhite( *pTemp ) && *pTemp ) 
				pTemp--;
			pTemp++;
			//if( isdigit( token[0] ) )
			//	tokenType = NUMBER;
			//if(token[0]=='0' && token[1]=='x') 
			//	tokenType = HEX;
		}
		*pTemp = '\0';
		Token = token;
	}
}
int CScanner::GetToken( BOOL bComma )
{
	if( m_bWideChar == TRUE )
		return GetToken_Wide( bComma );

//	char* m_pProg = m_pProg;
	char* pszTemp = m_mszToken; *pszTemp = '\0';
	tokenType = TEMP; tok = NONE_;
	m_nDoubleOps = 0;
BACK:
	// white space를 스킵
	if( 0 ) //bComma || m_bComma )
	{
		if( *m_pProg != '\r' && *m_pProg != '\0') 
		{
			while( iswhite( *m_pProg ) && *m_pProg ) 
				m_pProg++;
		}
		else
		{
			int a;
			a++;
		}
	}
	else
	while( iswhite( *m_pProg ) && *m_pProg ) 
		m_pProg++;

	while(*m_pProg=='/') // 주석문 처리
	{
		m_pProg++;
		if(*m_pProg=='/') 
		{ 
			m_pProg++;
			while(*m_pProg!='\r'&&*m_pProg!='\0') m_pProg++;
			if(*m_pProg=='\r') m_pProg+=2;
		}
		else
		if(*m_pProg=='*') 
		{
			m_pProg++;
			do 
			{
				while(*m_pProg!='*'&&*m_pProg!='\0') m_pProg++;
				// 파일의 끝이라면?
				if(*m_pProg=='\0') 
				{
					tok = FINISHED;
					tokenType = DELIMITER; 
					return tokenType;
				}
				m_pProg++;
			} while(*m_pProg!='/');
			m_pProg++;
			while(iswhite(*m_pProg) && *m_pProg) m_pProg++;
		}
		else 
		{
			m_pProg--;
			break;
		}
		goto BACK;
	}
	// 화일의 끝
	if(*m_pProg=='\0') 
	{
		tok = FINISHED;
		tokenType = DELIMITER; 
		goto EXIT;
	}
	// Comma 형식의 구분자인가?
	if( bComma || m_bComma )
	{
		// 콤마나 데이타의 끝, 또는 엔터 코드가 나올 때까지 읽는다.
		// 콤마 구분일 경우는 그것은 pszTemp와 NUMBER만을 구분한다.
		if(*m_pProg=='"') 
		{
			m_pProg++;
			while(*m_pProg!='"' && *m_pProg!='\r' && *m_pProg!='\0') *pszTemp++ = *m_pProg++;
			m_pProg++; 
			tokenType = STRING; 
		}
		else
		{
			while(*m_pProg!=',' && *m_pProg!='\r' && *m_pProg!='\0') 
				*pszTemp++ = *m_pProg++;
		}
		if( *m_pProg == ',' || *m_pProg == '\r')
			m_pProg++; // skim comma
		// white space를 스킵
		if( pszTemp != token )
		{
				pszTemp--;
			while( iswhite( *pszTemp ) && *pszTemp ) 
				pszTemp--;
			pszTemp++;
			if( isdigit( token[0] ) )
				tokenType = NUMBER;
			if(token[0]=='0' && token[1]=='x') 
				tokenType = HEX;
		}
		goto EXIT;
	}
	if(strchr("!<>=",*m_pProg)) 
	{ // is or might be a relation operator
		switch(*m_pProg) 
		{
			case '=':
				if(*(m_pProg+1)=='=') 
				{
					*pszTemp++ = *m_pProg++;
					*pszTemp++ = *m_pProg++;
					m_nDoubleOps = EQ;
				}
				break;
			case '!':
				if(*(m_pProg+1)=='=') 
				{
					*pszTemp++ = *m_pProg++;
					*pszTemp++ = *m_pProg++;
					m_nDoubleOps = NE;
				}
				else 
				{
					*pszTemp++ = *m_pProg++;
					m_nDoubleOps = NT;
				}
				break;
			case '<':
				if(*(m_pProg+1)=='=') 
				{
					*pszTemp++ = *m_pProg++;
					*pszTemp++ = *m_pProg++;
					m_nDoubleOps = LE;
				}
				else 
				{
					*pszTemp++ = *m_pProg++;
					m_nDoubleOps = LT;
				}
				break;
			case '>':
				if(*(m_pProg+1)=='=') 
				{
					*pszTemp++ = *m_pProg++;
					*pszTemp++ = *m_pProg++;
					m_nDoubleOps = GE;
				}
				else 
				{
					*pszTemp++ = *m_pProg++;
					m_nDoubleOps = GT;
				}
				break;
		}
		if(*token) { tokenType = DELIMITER; goto EXIT; }
	}
	// delimiter
	if(*m_pProg == '&' && *(m_pProg+1) == '&') 
	{
		*pszTemp = *(pszTemp+1) = '&';
		pszTemp+=2; m_pProg+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if(*m_pProg == '|' && *(m_pProg+1) == '|') 
	{
		*pszTemp = *(pszTemp+1) = '|';
		pszTemp+=2; m_pProg+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if( strchr( "+-*^/%=;(),':{}.", *m_pProg ) )  
	{
		*pszTemp = *m_pProg; m_pProg++;
		pszTemp++;	tokenType = DELIMITER; goto EXIT;
	}
	// 스트링 
	if(*m_pProg=='"') 
	{
		m_pProg++;
		while( *m_pProg!='"' && *m_pProg!='\r' && *m_pProg!='\0' && ( pszTemp - token ) < MAX_TOKENSTR ) 
		{
#if !defined(_MULTI_LANGUAGE_)
#ifdef __CHARNEXT
			CopyChar( &m_pProg, &pszTemp );
#else
			if( IsDBCSLeadByte( *m_pProg ) )
				*pszTemp++ = *m_pProg++;
			*pszTemp++ = *m_pProg++;
#endif
#else //_MULTI_LANGUAGE_
			int count = CopyChar( m_pProg, pszTemp );
			m_pProg += count;
			pszTemp += count;
#endif //_MULTI_LANGUAGE_
		}
		//while(*m_pProg!='"' && *m_pProg!='\r' && *m_pProg!='\0' ) *pszTemp++ = *m_pProg++;
//		if(*m_pProg=='\r'||*m_pProg=='\0') SntxErr(SYNTAX);
		//if( *m_pProg != '\0' )
		m_pProg++; 
		tokenType = STRING; 
		if( *(m_pProg-1 ) != '"' )
		{
			if( *(m_pProg-1 ) == '\0' )
				m_pProg--; 
			if( m_bErrorCheck )
			{
				CString string;
				if( ( pszTemp - token ) >= MAX_TOKENSTR )
					string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링 길이가 %d바이트를 초과했음.", GetLineNum(), m_szFileName, token, MAX_TOKENSTR );
				else
					string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링이 따옴표로 끝나지 않음.", GetLineNum(), m_szFileName, token );
				Error( string );
			}
		}
		goto EXIT;
	}
	// hex
	if( *m_pProg=='0' && *(m_pProg + 1 )=='x' ) 
	{
		m_pProg+=2;
		while(!isdelim(*m_pProg)) *pszTemp++ = *m_pProg++;
//		if(*m_pProg=='\r'||*m_pProg=='\0') SntxErr(SYNTAX);
		//m_pProg++; 
		tokenType = HEX; goto EXIT;
	}
	// 숫자
#if !defined(_MULTI_LANGUAGE_)
	if( isdigit( *m_pProg ) && !IsDBCSLeadByte( *m_pProg ) ) //숫자 검사만으로 끝나는데 왜 한글이 아닐 경우도 체크하지?
#else //_MULTI_LANGUAGE_
	if( isdigit( *m_pProg ) && !IsMultiByte( m_pProg ) ) //숫자 검사만으로 끝나는데 왜 한글이 아닐 경우도 체크하지?
#endif //_MULTI_LANGUAGE_
	{
		while( !isdelim( *m_pProg ) ) *pszTemp++ = *m_pProg++;
		tokenType = NUMBER; goto EXIT;
	}
	// 변수와 명령
#if !defined(_MULTI_LANGUAGE_)
	if( isalpha( *m_pProg ) || IsDBCSLeadByte( *m_pProg ) || *m_pProg == '#' || *m_pProg == '_' || *m_pProg == '@' || *m_pProg=='$' || *m_pProg == '?' ) 
#else //_MULTI_LANGUAGE_
	if( isalpha( *m_pProg ) || IsMultiByte( m_pProg ) || *m_pProg == '#' || *m_pProg == '_' || *m_pProg == '@' || *m_pProg=='$' || *m_pProg == '?' ) 
	//if( isalpha( *m_pProg ) || IsMultiByte( m_pProg ) || *m_pProg >= 0x80 ) || *m_pProg == '#' || *m_pProg == '_' || *m_pProg == '@' || *m_pProg=='$' || *m_pProg == '?' ) 
#endif //_MULTI_LANGUAGE_
	{
		while( !isdelim( *m_pProg ) ) 
		{
#if !defined(_MULTI_LANGUAGE_)
			if( IsDBCSLeadByte( *m_pProg ) )
				*pszTemp++ = *m_pProg++;

			*pszTemp++ = *m_pProg++;
#else //_MULTI_LANGUAGE_
			int count = CopyChar( m_pProg, pszTemp );
			m_pProg += count;
			pszTemp += count;
#endif //_MULTI_LANGUAGE_
		}
	}
	else
		*pszTemp++ = *m_pProg++;
	tokenType = TEMP;
EXIT:
	*pszTemp = '\0';
	Token = token; 
	return tokenType;
}
int CScanner::GetToken_Wide( BOOL bComma )
{
	wchar_t* pProg = (wchar_t*)m_pProg;
	wchar_t* pszTemp = m_wszToken; *pszTemp = '\0';
	tokenType = TEMP; tok = NONE_;
	m_nDoubleOps = 0;
BACK:
	// white space를 스킵
	if( 0 ) //bComma || m_bComma )
	{
		if( *pProg != '\r' && *pProg != '\0') 
		{
			while( iswhite( *pProg ) && *pProg ) 
				pProg++;
		}
		else
		{
			int a;
			a++;
		}
	}
	else
	while( iswhite( *pProg ) && *pProg ) 
		pProg++;

	while(*pProg=='/') // 주석문 처리
	{
		pProg++;
		if(*pProg=='/') 
		{ 
			pProg++;
			while(*pProg!='\r'&&*pProg!='\0') pProg++;
			if(*pProg=='\r') pProg+=2;
		}
		else
		if(*pProg=='*') 
		{
			pProg++;
			do 
			{
				while(*pProg!='*'&&*pProg!='\0') pProg++;
				// 파일의 끝이라면?
				if(*pProg=='\0') 
				{
					tok = FINISHED;
					tokenType = DELIMITER; 
					m_pProg = (char*)pProg;
					return tokenType;
				}
				pProg++;
			} while(*pProg!='/');
			pProg++;
			while(iswhite(*pProg) && *pProg) pProg++;
		}
		else 
		{
			pProg--;
			break;
		}
		goto BACK;
	}
	// 화일의 끝
	if(*pProg=='\0') 
	{
		tok = FINISHED;
		tokenType = DELIMITER; 
		goto EXIT;
	}
	// Comma 형식의 구분자인가?
	if( bComma || m_bComma )
	{
		// 콤마나 데이타의 끝, 또는 엔터 코드가 나올 때까지 읽는다.
		// 콤마 구분일 경우는 그것은 pszTemp와 NUMBER만을 구분한다.
		if(*pProg=='"') 
		{
			pProg++;
			while(*pProg!='"' && *pProg!='\r' && *pProg!='\0') *pszTemp++ = *pProg++;
			pProg++; 
			tokenType = STRING; 
		}
		else
		{
			while(*pProg!=',' && *pProg!='\r' && *pProg!='\0') 
				*pszTemp++ = *pProg++;
		}
		if( *pProg == ',' || *pProg == '\r')
			pProg++; // skim comma
		// white space를 스킵
		if( pszTemp != m_wszToken )
		{
				pszTemp--;
			while( iswhite( *pszTemp ) && *pszTemp ) 
				pszTemp--;
			pszTemp++;
			if( isdigit( m_wszToken[0] ) )
				tokenType = NUMBER;
			if(m_wszToken[0]=='0' && m_wszToken[1]=='x') 
				tokenType = HEX;
		}
		goto EXIT;
	}
	if(strchr("!<>=",*pProg)) 
	{ // is or might be a relation operator
		switch(*pProg) 
		{
			case '=':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = EQ;
				}
				break;
			case '!':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = NE;
				}
				else 
				{
					*pszTemp++ = *pProg++;
					m_nDoubleOps = NT;
				}
				break;
			case '<':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = LE;
				}
				else 
				{
					*pszTemp++ = *pProg++;
					m_nDoubleOps = LT;
				}
				break;
			case '>':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = GE;
				}
				else 
				{
					*pszTemp++ = *pProg++;
					m_nDoubleOps = GT;
				}
				break;
		}
		if(m_wszToken[0]) { tokenType = DELIMITER; goto EXIT; }
	}
	// delimiter
	if(*pProg == '&' && *(pProg+1) == '&') 
	{
		*pszTemp = *(pszTemp+1) = '&';
		pszTemp+=2; pProg+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if(*pProg == '|' && *(pProg+1) == '|') 
	{
		*pszTemp = *(pszTemp+1) = '|';
		pszTemp+=2; pProg+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if( strchr( "+-*^/%=;(),':{}.", *pProg ) )  
	{
		*pszTemp = *pProg; pProg++;
		pszTemp++;	tokenType = DELIMITER; goto EXIT;
	}
	// 스트링
	if(*pProg=='"') 
	{
		pProg++;
		while(*pProg!='"' && *pProg!='\r' && *pProg!='\0' && ( pszTemp - m_wszToken ) < MAX_TOKENSTR ) 
			*pszTemp++ = *pProg++;
		//while(*pProg!='"' && *pProg!='\r' && *pProg!='\0' ) *pszTemp++ = *pProg++;
//		if(*pProg=='\r'||*pProg=='\0') SntxErr(SYNTAX);
		//if( *pProg != '\0' )
		pProg++; 
		tokenType = STRING; 
		if( *(pProg-1 ) != '"' )
		{
			if( *(pProg-1 ) == '\0' )
				pProg--; 
			if( m_bErrorCheck )
			{
				CString string;
				if( ( pszTemp - m_wszToken ) >= MAX_TOKENSTR )
					string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링 길이가 %d바이트를 초과했음.", GetLineNum(), m_szFileName, token, MAX_TOKENSTR );
				else
					string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링이 따옴표로 끝나지 않음.", GetLineNum(), m_szFileName, token );
				Error( string );
			}
		}
		goto EXIT;
	}
	// hex
	if( *pProg=='0' && *(pProg + 1 )=='x' ) 
	{
		pProg+=2;
		while(!isdelim(*pProg)) *pszTemp++ = *pProg++;
//		if(*pProg=='\r'||*pProg=='\0') SntxErr(SYNTAX);
		//pProg++; 
		tokenType = HEX; goto EXIT;
	}
	// 숫자
	if( isdigit( *pProg ) && !IsDBCSLeadByte( BYTE(*pProg) ) ) //숫자 검사만으로 끝나는데 왜 한글이 아닐 경우도 체크하지?
	{
		while( !isdelim( *pProg ) ) *pszTemp++ = *pProg++;
		tokenType = NUMBER; goto EXIT;
	}
	// 변수와 명령
	if( isalpha( *pProg ) || IsDBCSLeadByte( BYTE(*pProg) ) || *pProg == '#' || *pProg == '_' || *pProg == '@' || *pProg=='$' || *pProg == '?' ) 
	{
		while(!isdelim(*pProg)) *pszTemp++ = *pProg++;
	}
	else
		*pszTemp++ = *pProg++;
	tokenType = TEMP;
EXIT:
	m_pProg = (char*)pProg;
	*pszTemp = '\0';
	Token = m_wszToken; 
#if !defined(_MULTI_LANGUAGE_)
	int nLength = wcslen( m_wszToken ); 
	_wcstombsz( m_mszToken, m_wszToken, ( nLength * 2 ) + 1 );
#else //_MULTI_LANGUAGE_
	WideCharToMultiByte(g_codePage, 0, m_wszToken, -1, m_mszToken, sizeof(m_mszToken), NULL, NULL);
#endif //_MULTI_LANGUAGE_
	return tokenType;
}

int CScanner::GetNumber( BOOL bComma )
{
	m_dwDef = 1;
	if( GetToken( bComma ) == HEX )
	{
		Token.MakeLower();
		DWORDLONG dwlNumber = 0;
		DWORD dwMulCnt = 0;
		CHAR cVal;
		for( int i = Token.GetLength() - 1; i >= 0; i--)
		{
			cVal = Token[ i ];
			if( cVal >= 'a' )
				cVal = ( cVal - 'a' ) + 10;
			else cVal -= '0';
			dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
			dwMulCnt += 4;
		}
		m_dwDef = 0;
		return (DWORD)dwlNumber;
	}
	else
	if(!Token.IsEmpty())
	{
		switch(Token[0])
		{
			case '=': 
				m_dwDef = 0;
				return NULL_ID;
			case '-': 
				if( bComma == FALSE )
				{
					GetToken();	
					m_dwDef = 0;
					return -atoi(Token);
				}
				else
				{
					m_dwDef = 0;
					return atoi(Token);
				}
			case '+':	
				if( bComma == FALSE )
					GetToken();	
		}
		m_dwDef = 0;
		return atoi(Token);
	}
	m_dwDef = 0;
	return 0;
}

__int64 CScanner::GetInt64( BOOL bComma )
{
	m_dwDef		= 1;
	if( GetToken( bComma ) == HEX )
	{
		/*
		Token.MakeLower();
		DWORDLONG dwlNumber = 0;
		DWORD dwMulCnt = 0;
		CHAR cVal;
		for( int i = Token.GetLength() - 1; i >= 0; i--)
		{
			cVal = Token[ i ];
			if( cVal >= 'a' )
				cVal = ( cVal - 'a' ) + 10;
			else cVal -= '0';
			dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
			dwMulCnt += 4;
		}
		m_dwDef = 0;
		return (DWORD)dwlNumber;
		*/
	}
	else if( !Token.IsEmpty() )
	{
		switch( Token[0] )
		{
			case '=': 
				m_dwDef = 0;
				return NULL_ID;
			case '-': 
				if( bComma == FALSE )
				{
					GetToken();	
					m_dwDef = 0;
					return -_atoi64( Token );
				}
				else
				{
					m_dwDef = 0;
					return _atoi64( Token );
				}
			case '+':	
				if( bComma == FALSE )
					GetToken();	
		}
		m_dwDef = 0;
		return _atoi64( Token );
	}
	m_dwDef = 0;
	return (__int64)0;
}

FLOAT CScanner::GetFloat( BOOL bComma )
{
	m_dwDef = 1;
	GetToken( bComma );
	if( !Token.IsEmpty() )
	{
		switch( Token[0] )
		{
			case '=': 
				m_dwDef = 0;
				return -1;
			case '-': 
				GetToken(bComma);	
				m_dwDef = 0;
				return (FLOAT)-atof(Token);
			case '+':	
				GetToken(bComma);	
		}
		m_dwDef = 0;
		return (FLOAT)atof(Token);
	}
	m_dwDef = 0;
	return 0.0f;
}
DWORD CScanner::GetHex( BOOL bComma )
{
	m_dwDef = 1;
	if( GetToken( bComma ) == HEX )
	{
		Token.MakeLower();
		DWORDLONG dwlNumber = 0;
		DWORD dwMulCnt = 0;
		CHAR cVal;
		for( int i = Token.GetLength() - 1; i >= 0; i--)
		{
			cVal = Token[ i ];
			if( cVal >= 'a' )
				cVal = ( cVal - 'a' ) + 10;
			else cVal -= '0';
			dwlNumber |= (DWORDLONG) cVal << dwMulCnt;
			dwMulCnt += 4;
		}
		m_dwDef = 0;
		return (DWORD)dwlNumber;
	}
	m_dwDef = 0;
	return 0;
}
CScanner& CScanner::operator>>(int& i)
{
	i = GetNumber();
	return *this;
}
CScanner& CScanner::operator>>(float& f)
{
	f = GetFloat();
	return *this;
}
/*
CScanner& CScanner::operator>>( LPWSTR lpwszStr )
{
	GetToken();
	//strcpy( lpszStr, Token );
	return *this;
}
*/
/*
	int a,b,c;
	TCHAR arg1[10];
	TCHAR arg2[10];
	CScanner scanner( lpData );
	scanner >> a >> b >> c >> arg1 >> arg2;
*/
void CScanner::SkipToken( int nNumber, BOOL bComma )
{
	for( int i = 0; i < nNumber; i++ )
		GetToken( bComma );
}
void CScanner::PutBack()
{ 
	if( m_bWideChar )
	{
		for( const wchar_t *t = m_wszToken; *t; t++ ) 
			m_pProg-=2;
		if( tokenType == STRING ) // 따옴표 처리 
			m_pProg-=4;
	}
	else
	{
		for( const char *t = m_mszToken; *t; t++ ) 
			m_pProg--;
		if( tokenType == STRING ) // 따옴표 처리 
			m_pProg-=2;
	}
}
#ifdef __SCRIPT_TOOL
int CScanner::GetToken2( BOOL bComma )
{
	wchar_t* pProg = (wchar_t*)m_pProg;
	wchar_t* pszTemp = m_wszToken; *pszTemp = '\0';
	tokenType = TEMP; tok = NONE_;
	m_nDoubleOps = 0;
BACK:
	// white space를 스킵
	if( 0 ) //bComma || m_bComma )
	{
		if( *pProg != '\r' && *pProg != '\0') 
		{
			while( iswhite( *pProg ) && *pProg ) 
				pProg++;
		}
		else
		{
			int a;
			a++;
		}
	}
	else
	while( iswhite( *pProg ) && *pProg ) 
		pProg++;
/*
	while(*pProg=='/') // 주석문 처리
	{
		pProg++;
		if(*pProg=='/') 
		{ 
			pProg++;
			while(*pProg!='\r'&&*pProg!='\0') pProg++;
			if(*pProg=='\r') pProg+=2;
		}
		else
		if(*pProg=='*') 
		{
			pProg++;
			do 
			{
				while(*pProg!='*'&&*pProg!='\0') pProg++;
				// 파일의 끝이라면?
				if(*pProg=='\0') 
				{
					tok = FINISHED;
					tokenType = DELIMITER; 
					m_pProg = (char*)pProg;
					return tokenType;
				}
				pProg++;
			} while(*pProg!='/');
			pProg++;
			while(iswhite(*pProg) && *pProg) pProg++;
		}
		else 
		{
			pProg--;
			break;
		}
		goto BACK;
	}
	*/
	// 화일의 끝
	if(*pProg=='\0') 
	{
		tok = FINISHED;
		tokenType = DELIMITER; 
		goto EXIT;
	}
	// Comma 형식의 구분자인가?
	if( bComma || m_bComma )
	{
		// 콤마나 데이타의 끝, 또는 엔터 코드가 나올 때까지 읽는다.
		// 콤마 구분일 경우는 그것은 pszTemp와 NUMBER만을 구분한다.
		if(*pProg=='"') 
		{
			pProg++;
			while(*pProg!='"' && *pProg!='\r' && *pProg!='\0') *pszTemp++ = *pProg++;
			pProg++; 
			tokenType = STRING; 
		}
		else
		{
			while(*pProg!=',' && *pProg!='\r' && *pProg!='\0') 
				*pszTemp++ = *pProg++;
		}
		if( *pProg == ',' || *pProg == '\r')
			pProg++; // skim comma
		// white space를 스킵
		if( pszTemp != m_wszToken )
		{
				pszTemp--;
			while( iswhite( *pszTemp ) && *pszTemp ) 
				pszTemp--;
			pszTemp++;
			if( isdigit( m_wszToken[0] ) )
				tokenType = NUMBER;
			if(m_wszToken[0]=='0' && m_wszToken[1]=='x') 
				tokenType = HEX;
		}
		goto EXIT;
	}
	if(strchr("!<>=",*pProg)) 
	{ // is or might be a relation operator
		switch(*pProg) 
		{
			case '=':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = EQ;
				}
				break;
			case '!':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = NE;
				}
				else 
				{
					*pszTemp++ = *pProg++;
					m_nDoubleOps = NT;
				}
				break;
			case '<':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = LE;
				}
				else 
				{
					*pszTemp++ = *pProg++;
					m_nDoubleOps = LT;
				}
				break;
			case '>':
				if(*(pProg+1)=='=') 
				{
					*pszTemp++ = *pProg++;
					*pszTemp++ = *pProg++;
					m_nDoubleOps = GE;
				}
				else 
				{
					*pszTemp++ = *pProg++;
					m_nDoubleOps = GT;
				}
				break;
		}
		if(m_wszToken[0]) { tokenType = DELIMITER; goto EXIT; }
	}
	// delimiter
	if(*pProg == '&' && *(pProg+1) == '&') 
	{
		*pszTemp = *(pszTemp+1) = '&';
		pszTemp+=2; pProg+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if(*pProg == '|' && *(pProg+1) == '|') 
	{
		*pszTemp = *(pszTemp+1) = '|';
		pszTemp+=2; pProg+=2; tokenType = DELIMITER; goto EXIT; 
	}
	if( strchr( "+-*^/%=;(),':{}.", *pProg ) )  
	{
		*pszTemp = *pProg; pProg++;
		pszTemp++;	tokenType = DELIMITER; goto EXIT;
	}
	// 스트링
	if(*pProg=='"') 
	{
		pProg++;
		while(*pProg!='"' && *pProg!='\r' && *pProg!='\0' && ( pszTemp - m_wszToken ) < MAX_TOKENSTR ) 
			*pszTemp++ = *pProg++;
		//while(*pProg!='"' && *pProg!='\r' && *pProg!='\0' ) *pszTemp++ = *pProg++;
//		if(*pProg=='\r'||*pProg=='\0') SntxErr(SYNTAX);
		//if( *pProg != '\0' )
		pProg++; 
		tokenType = STRING; 
		if( *(pProg-1 ) != '"' )
		{
			if( *(pProg-1 ) == '\0' )
				pProg--; 
#if !defined( __WORLDSERVER ) && !defined( __MASTER ) 		
			CString string;
			if( ( pszTemp - m_wszToken ) >= MAX_TOKENSTR )
				string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링 길이가 %d바이트를 초과했음.", GetLineNum(), m_szFileName, token, MAX_TOKENSTR );
			else
				string.Format( "line(%d) 파일 %s에서 \"%s\" 스트링이 따옴표로 끝나지 않음.", GetLineNum(), m_szFileName, token );
			AfxMessageBox( string );
#endif
		}
		goto EXIT;
	}
	// hex
	if( *pProg=='0' && *(pProg + 1 )=='x' ) 
	{
		pProg+=2;
		while(!isdelim(*pProg)) *pszTemp++ = *pProg++;
//		if(*pProg=='\r'||*pProg=='\0') SntxErr(SYNTAX);
		//pProg++; 
		tokenType = HEX; goto EXIT;
	}
	// 숫자
	if( isdigit( *pProg ) && !IsDBCSLeadByte( *pProg ) ) //숫자 검사만으로 끝나는데 왜 한글이 아닐 경우도 체크하지?
	{
		while( !isdelim( *pProg ) ) *pszTemp++ = *pProg++;
		tokenType = NUMBER; goto EXIT;
	}
	// 변수와 명령
	if( isalpha( *pProg ) || IsDBCSLeadByte( *pProg ) || *pProg == '#' || *pProg == '_' || *pProg == '@' || *pProg=='$' || *pProg == '?' ) 
	{
		while(!isdelim(*pProg)) *pszTemp++ = *pProg++;
	}
	else
		*pszTemp++ = *pProg++;
	tokenType = TEMP;
EXIT:
	m_pProg = (char*)pProg;
	*pszTemp = '\0';
	Token = m_wszToken; 
#if !defined(_MULTI_LANGUAGE_)
	int nLength = wcslen( m_wszToken ); 
	_wcstombsz( m_mszToken, m_wszToken, ( nLength * 2 ) + 1 );
#else //_MULTI_LANGUAGE_
	WideCharToMultiByte(g_codePage, 0, m_wszToken, -1, m_mszToken, sizeof(m_mszToken), NULL, NULL);
#endif //_MULTI_LANGUAGE_
	return tokenType;
}
#endif //__SCRIPT_TOOL
