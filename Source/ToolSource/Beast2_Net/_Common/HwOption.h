#ifndef __HWOPTION_H__
#define	__HWOPTION_H__

class COption
{
public:
	TCHAR	m_szFileName[128];
	int		m_nLanguage;
//	int		m_nSoundEffect;
	int		m_nResWidth;
	int		m_nResHeight;
	BOOL	m_bStartFullScreen;
//	float	m_fMusicVolume;
	int		m_nTextureQuality;		// 텍스쳐 품질 높음0,중간1,낮음2
	int		m_nViewArea;			// 시야범위 멀리0,중간1,가깝게2
	int		m_nObjectDetail;		// 오브젝트 표현수준 높음0,중간1,낮음2
	int     m_nObjectDistant;       // 오브젝트 거리  높음0,중간1,낮음2
	int     m_nShadow; // 그림자 
	int		m_nToggleScreen;		// 알트엔터 가능/못가능.  이건 유저들은 모른다.
	BOOL    m_bOperator;

#ifdef __VCHATFILTER_20050316
	DWORD   m_dwChatFilter[5];
	DWORD   m_dwChatFilterEtc;
#else
	DWORD   m_dwChatFilter;
#endif

#ifdef __XGLARE
	int		m_nBloom;				// 뽀샤시 효과.
#endif
	
#ifdef __S0810_NOTMONNAME
	int		m_nMonName;		// 몬스터 이름 On / Off
#endif // __S0810_NOTMONNAME

#ifdef __TOOLTIPTEXTURE
	int		m_nToolTipTexture;		// 툴팁 인터페이스 바꾸기
#endif // __TOOLTIPTEXTURE
	int		m_nToolTipText;

#ifdef __DEV1020
	BOOL	m_bDev;
#endif	// __DEV1020
	


//#ifdef __YADDSOUND
	BOOL    m_bVoice;
	float	m_fEffectVolume;
	float	m_fBGMVolume;
//#endif //__YADDSOUND

	// 윈도 옵션
	int     m_nWindowAlpha; // 0 ~ 255
	int     m_nWindowEffect; // 0(일반), 1(반투명), 2(사각)

	// 속어 단어 필터
	int     m_nSlangWord; // 0(교체), 1(숨김)

	// 채팅 명령어
	int     m_nChatCommand; // 0(한글 풀문장),1(한글축약어),2(영문풀문장),3(영문축약어)
	
	// 초보자 도움말(채팅창에 나오는 것)
	int     m_nInstantHelp; // 
	
	// 인터페이스 방식
	int		m_nInterface;	// 0 : 구버전 1 : 신버전(우클릭으로 카메라 회전하는..)
	BOOL	m_bRollEffect;	// 비행시 롤링 효과.
	
	BOOL	m_bSay;			// 귀속말 승인 / 거부
	BOOL	m_bTrade;		// 거래 승인 / 거부
	BOOL	m_bParty;		// 파티 승인 / 거부
	BOOL	m_bShout;		// 외치기 표시 ON / OFF
	BOOL	m_bMessengerJoin; // Messenger 알림 표시 On / Off
	BOOL	m_bMessenger;	// Messenger 승인 / 거부

	int		m_nSer;		// 이전에 접속했던 서버
	int		m_nMSer;	// 이전에 접속했던 멀티 서버
	
	BOOL    m_bSaveAccount;
	TCHAR   m_szAccount[ 64 ];
	BOOL    m_bNotice;
	BOOL    m_bTip;
	time_t  m_tNoticeTime;
	
#ifdef __YGUIDESYSTEM0923
	int		m_nGuide1;	
	int		m_nGuide2;	
	int		m_nGuide3;	
#endif //__YGUIDESYSTEM0923
	
	BOOL    m_bIsShowReSkillMessage;

#ifdef __H1207_IPSET__
	TCHAR	m_IPAddress[32];
	TCHAR	m_szRegistry[256];
#endif

#ifdef __YENV
	BOOL	m_bExitChange;
	BOOL	m_bSpecBump;
#endif //__YENV

#ifdef __YDAMAGE_RENDER
	BOOL	m_bDamageRender;
#endif //__YDAMAGE_RENDER	

#ifdef __YVISIBLE_BUFF_TIME
	BOOL	m_bVisibleBuffTimeRender;
#endif //__YDAMAGE_RENDER	
	
#ifdef __ACROBAT_0504
	int		m_MouseSpeed;
#endif //__ACROBAT_0504

#ifdef __YRENDER_FLAG	
	BOOL	m_bSFXRenderOff;	
#endif //__YRENDER_FLAG	

	int*	m_pGuide;
	
	COption();
	~COption();
	
	void Init();
	int Load( LPCTSTR szFileName );
	int Save( LPCTSTR szFileName );
#ifdef __SDEBUG_TOOLTIP_TEST
	int LoadToolTip( LPCTSTR szFileName );
#endif // __SDEBUG_TOOLTIP_TEST
};

extern COption		g_Option;

#endif
