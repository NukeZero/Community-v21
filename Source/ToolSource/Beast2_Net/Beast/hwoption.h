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

	DWORD   m_dwChatFilter;

	int		m_nToolTipText;

	BOOL    m_bVoice;
	float	m_fEffectVolume;
	float	m_fBGMVolume;

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
	
	
	BOOL    m_bIsShowReSkillMessage;


	int		m_MouseSpeed;

#ifdef __YRENDER_FLAG	
	BOOL	m_bSFXRenderOff;	
#endif //__YRENDER_FLAG	

	int*	m_pGuide;
	
	COption();
	~COption();
	
	void Init();
	int Load( LPCTSTR szFileName );
	int Save( LPCTSTR szFileName );
};

extern COption		g_Option;

#endif
