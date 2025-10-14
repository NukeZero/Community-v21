#ifndef _GG_AUTH_SERVER_H_
#define _GG_AUTH_SERVER_H_

#ifdef _EXPORT_DLL
	#define GGAUTHS_API __declspec(dllexport)
#else
	#define GGAUTHS_API 
#endif

// gameguard auth data
typedef struct _GG_AUTH_DATA
{
	DWORD dwIndex;
	DWORD dwValue1;
	DWORD dwValue2;
	DWORD dwValue3;
} GG_AUTH_DATA, *PGG_AUTH_DATA;

typedef struct _GG_AUTH_PROTOCOL *PGG_AUTH_PROTOCOL;

// ggauth.dll¿« Path
extern "C" GGAUTHS_API DWORD __cdecl InitGameguardAuth(char* sGGPath, DWORD dwNumActive = 50);
extern "C" GGAUTHS_API void  __cdecl CleanupGameguardAuth();

// protocol dll name
extern "C" GGAUTHS_API DWORD __cdecl AddAuthProtocol(char* sDllName);

// Class CSAuth2
class GGAUTHS_API CCSAuth2
{
public:
	// Constructor
	CCSAuth2();

	// Destructor
	~CCSAuth2();

protected:
	PGG_AUTH_PROTOCOL m_pProtocol;
	DWORD m_bPrtcRef;
	DWORD m_dwUserFlag;

public:	
	GG_AUTH_DATA m_AuthQuery;
	GG_AUTH_DATA m_AuthAnswer;	

	DWORD __cdecl GetAuthQuery();
	DWORD __cdecl CheckAuthAnswer();	
};

// C type CSAuth2
typedef LPVOID          LPGGAUTH;

extern "C" GGAUTHS_API LPGGAUTH __cdecl GGAuthInitUser();                                              // CCSAuth2()
extern "C" GGAUTHS_API void     __cdecl GGAuthCloseUser(LPGGAUTH pGGAuth);                             // ~CCSAuth2()
extern "C" GGAUTHS_API DWORD    __cdecl GGAuthGetQuery(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);     // GetAuthQuery()
extern "C" GGAUTHS_API DWORD    __cdecl GGAuthCheckAnswer(LPGGAUTH pGGAuth, PGG_AUTH_DATA pAuthData);  // CheckAuthAnswer()

#endif