#ifndef __NEUZAPP_H 
#define __NEUZAPP_H

#define WM_LOGOUT ( WM_USER + 10 )


#define CUR_ATTACK       0
#define CUR_BASE         1
#define CUR_CAMERA       2
#define CUR_CONTROL      3
#define CUR_DELAY        4
#define CUR_EDIT         5
#define CUR_GETITEM      6
#define CUR_HELP         7
#define CUR_NO           8
#define CUR_NPC          9
#define CUR_SELECT       10
#define CUR_RESIZE_HORI  11
#define CUR_RESIZE_VERT  12
#define CUR_RESIZE_HV1   13
#define CUR_RESIZE_HV2   14
#define CUR_DOOR	     15
#define CUR_GATE	     16
#define CUR_LEVER        17
#define CUR_HAMMER       18

struct LOGIN_STEP_INFO
{
	LPCTSTR		pszAccount;			// account
	LPCTSTR		pszPassword;		// password
	DWORD		dwID;				// pServerDesc->dwID 서버번호 
	int			nCount;				// 
};

#include "NeuzEnemy.h"



class CNeuzApp : public CD3DApplication 
{
public:
	CNeuzApp();

public:
	DWORD					m_dwTimeOutDis;
	int						m_nTexAlpha;
	BOOL					m_bTexLoad;
	BOOL					m_bTexLoadAlpha;
	CTexture				m_TexLoading;
	BOOL					m_bActiveNeuz;
	CNeuzEnemy				m_NeuzEnemy;
	DWORD					m_dwTimeReSkillPoint;
	BOOL					m_bQuit;
	CTimer					m_timerQuit;//.Set( SEC( 1 ) );	
	CTexturePack			m_texQuestEmoticon;
	C2DRender				m_2DRender;
	HCURSOR					m_ahCursor[ 20 ];
	TCHAR					m_szCaptureFileName[ 64 ];
	BOOL					m_bCapture;
	int						m_nCaptureCnt;
	CTimer					m_timerConnect;
	DWORD					m_dwTempMessage;
	BOOL					m_bLButtonDown;
	BOOL					m_bRButtonDown;
	CBackCamera				m_camera;
	BOOL					m_bFrameSkip;			// 프레임 스키핑 플래그
	CTexture				m_TextureGauge[5];
	CTexture				m_TextureTurboGauge[2];
	CTexture				m_TextureCastingGauge[2];
	CTexturePack			m_TexturePackPVP;
	int						m_nPVPCount;
	DWORD					m_dwPVPTime;
	CStringArray			m_strArray;
	TCHAR					m_lpCacheAddr[16];
	LOGIN_STEP_INFO			m_loginSI;
	TCHAR					m_szAccount[MAX_ACCOUNT];			// account

	//////////////////////////////////////////////////////////////////////////
	// mirchang_100727 일본 한게임

	TCHAR					m_szPassword[MAX_ENCRYPT_PWD];			// password

	//////////////////////////////////////////////////////////////////////////
	BOOL	m_bGPotatoAuth;
	TCHAR	m_szGPotatoNo[MAX_ACCOUNT];
	TCHAR	m_szGPotatoNo2[MAX_ACCOUNT];
	TCHAR	m_szCheck[255];
	DWORD					m_dwSys;
	DWORD					m_dwAuthKey;
	CMover*					m_apPlayer[MAX_CHARACTER_SLOT];
	DWORD					m_adwWorldID[MAX_CHARACTER_SLOT];
	int						m_nCharacterBlock[MAX_CHARACTER_SLOT];
	CRTMessenger			m_aRTMessenger[MAX_CHARACTER_SLOT];

	BOOL	m_bThroughPortal;

	TCHAR	m_szSessionPwd[MAX_SESSION_PWD];
	u_long					m_idPlayer;
	int						m_nTagCount;
	BOOL					m_bCheckTag;
	char					m_strTagName[ 20 ][ 65 ];
	DWORD					m_dwtegDate[ 20 ];
	char					m_strTagMessage[ 20 ][ 256 ];
	DWORD					m_dwPingTime;
	short					m_nPingCount;
	BYTE					m_cbAccountFlag;	// 18세이상, 학교대항전, 24시간플레이 플래그 
	DWORD					m_dwCurTick;
	DWORD					m_dwTimeLeft;		// 플레이할 수 있게 남은 시간 (태국의 경우)
	UINT					m_nLeftTimeNotifyStatus;
	u_long					m_uIdofMulti;
	BOOL					m_b18Server;		
	HANDLE					m_hThread;
	DWORD					m_dwSummonPartyData;
	OBJID					m_dwSummonPartyObjid;
	char					m_szSummonPartyWorldName[128];
	CTexture				m_TextureHPGauge[3];
	CTexture*				m_pMasterIcon[6];
	CTexture*				m_pHeroIcon;

	CTexture				m_TexCltGauge[2];		//sun: 11, 채집 시스템

	FLItemBase				m_aSavedInven[MAX_VENDITEM];	//sun: 11, 개인상점 개선

	int						m_n2ndPasswordNumber;

	BYTE					m_byTotalCharNum;				// 총 캐릭터 수
	BYTE					m_byTotalCharSelectPage;		// 캐릭터 선택 페이지 수
	BYTE					m_byCurrentCharSelectPage;		// 현재 캐릭터 선택 페이지

#ifdef PASSWORD_RESET_2ND
	int						m_nResetPassword[MAX_CHARACTER_SLOT];
	CString					m_strNewPasswordNumber;

#endif // PASSWORD_RESET_2ND
	

	int						m_certifySiteValue;				// 일본 인증 채널 선택

	enum GameGradeMarkState { FADE_IN, SHOW, FADE_OUT };
	enum { GAME_GRADE_MARK_MINIMUM_ALPHA = 0, GAME_GRADE_MARK_MAXIMUM_ALPHA = 255 };
	enum { GAME_GRADE_MARK_RENDERING_INTERVAL_SECOND = 3600 };
	enum { GAME_GRADE_MARK_SHOWING_SECOND = 3 };

	void CalculateGameGradeMarkAlpha( void );

	static const FLOAT GAME_GRADE_MARK_FADING_SPEED;

	DWORD m_dwTimeGameGradeMarkRendering;
	BOOL m_bGameGradeMarkRenderingSwitch;
	int m_nGameGradeMarkAlpha;
	GameGradeMarkState m_eGameGradeMarkState;
	DWORD m_dwTimeOld;
	DWORD m_dwTimeGameGradeMarkShowing;

	CTexture				m_TextureAngelGauge[2];		//sun: 8차 엔젤 소환 Neuz, World, Trans
	TCHAR					m_szWhisperName[MAX_NAME];			// 귓속말 보낼 이름
	
//sun: 12, 몬스터 생성 Neuz, World
	D3DXVECTOR3				m_vCursorPos;
	FLItemElem*				m_pCreateMonItem;

#ifdef __YENV
	LPD3DXEFFECT			m_pEffect;
	D3DXHANDLE				m_hTechnique;
	D3DXHANDLE				m_hmWVP;
	D3DXHANDLE				m_hvLightDir;

	D3DXHANDLE				m_hTexDiffuse;
	D3DXHANDLE				m_hTexBump;
	D3DXHANDLE				m_hTexSpecular;

	D3DXHANDLE				m_hvEyePos;
	D3DXHANDLE				m_hvFog;
	D3DXHANDLE				m_hvEye;
	D3DXHANDLE				m_hvDiffuse;
	D3DXHANDLE				m_hvSpecual;	
	D3DXHANDLE				m_hvAmbient;
	
	LPDIRECT3DTEXTURE9		m_pEnvTex;
#endif //__YENV	

	CWndBase* pOldModalWindow;

	//  [10/22/2014 ChangSeop.Kim]
	std::string	m_InGameShopURL;
	//  [10/22/2014 ChangSeop.Kim]

protected:
	HRESULT		OneTimeSceneInit();
	HRESULT		InitDeviceObjects();
	HRESULT		RestoreDeviceObjects();
	HRESULT		InvalidateDeviceObjects();
	HRESULT		DeleteDeviceObjects();
	HRESULT		FinalCleanup();
	HRESULT		Render(); 
	HRESULT		FrameMove();
	HRESULT		ConfirmDevice( D3DCAPS9* pCaps, DWORD dwBehavior, 
								D3DFORMAT adapterFormat, D3DFORMAT backBufferFormat );
	void		NotifyLeftTime();
	void		NotifyLeftMinute( UINT type, int nMin );
	void		HashMD5( char* szResult, size_t cchResult, LPCTSTR szPWD );
	void		InitSecurity();

private:
	void		InitNProtect();		// TODO : 게임가드쪽으로 분리

public:

	void		ResetStaticValues( );	//gmpbigsun(100429)

	void Drv_SetGamma(HWND hWnd, float gamma, int overbright, float fContrast);	//sun: 밝기, 감마, 명암 조절 Neuz

	LRESULT		MsgProc( HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam );
	void		SetLeftTime( long lTimeLeft );
	HRESULT		CaptureBitmap();	   

	void		SetAccountInfo( LPCTSTR lpszAccount, LPCTSTR lpszpw, LPCTSTR lpszSessionPwd = "", bool bEncrypt = true );

	void		GetDeleteKey( LPCTSTR szNo, TCHAR* deleteKey, const size_t lenDeleteKey );
	BOOL		LoadOption();
	BOOL		KillWindow( int wParam );
	void		BeginLoadThread();	
	void		WaitLoading();
	void		FinalLoaing( );			//gmpbigsun( 20120703 ) : 마지막 로딩작업!

	//gmpbigsun(20111122) : check model
#ifdef _DEBUG
	BOOL		LoadAllModel( );		//mdldyna에 등록된 모든 모델 로드
#endif

};

extern void SetMouseCursor( DWORD dwCursor );

extern CNeuzApp g_Neuz;

#endif