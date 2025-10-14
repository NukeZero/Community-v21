#ifndef __RESPAWNER_H__
#define __RESPAWNER_H__

#define RESPAWNTYPE_REGION   0
#define RESPAWNTYPE_SCRIPT   1
#define RESPAWNTYPE_BACKEND  2

#define			MAX_CTRLDROPITEM		4
#define			MAX_CTRLDROPMOB			3
#define			MAX_TRAP                3

#define			MAX_KEY					64

typedef struct CCtrlElem
{
	DWORD		m_dwSet;				    //-열기조건 설정 UA_ITEM, UA_LEVEL...
	DWORD 		m_dwSetItem;				//-박스 오픈 조건(아이템)								- UA_ITEM
	DWORD		m_dwSetLevel;				//-박스 오픈 조건(레벨)									- UA_LEVEL
	DWORD		m_dwSetQuestNum;			//-박스 오픈 조건(퀘스트 넘버,퀘스트 플래그 넘버)       - UA_QUEST
	DWORD		m_dwSetFlagNum;
	DWORD		m_dwSetGender;				//-박스 오픈 조건(성별)									- UA_GENDER

	///----------------------------------------------------------------------------------------
	//gmpbigsun: 16차에 40으로 늘어났지만 데이터 문제로 인해 32로 고정시킴
	BOOL		m_bSetJob[32];			//-박스 오픈 조건(클래스)									- UA_CLASS
	///----------------------------------------------------------------------------------------
	
	DWORD		m_dwSetEndu;				//-박스 내구도 ? hp와 같이 적용	
	 
	DWORD		m_dwMinItemNum;				//-최소 발생 아이템 수 
	DWORD		m_dwMaxiItemNum;			//-최대 발생 아이템 수 
	DWORD		m_dwInsideItemKind[MAX_CTRLDROPITEM];		//-들어가는 아이템 종류 값 !!!주의!!! 마지막 MAX_CTRLDROPITEM-1 은 레어 아템공간
	DWORD		m_dwInsideItemPer [MAX_CTRLDROPITEM];		//-들어가는 아이템 의 발생 확룰( 3000000000 )
	
	DWORD       m_dwMonResKind  [MAX_CTRLDROPMOB];          //-몬스터1 발생 종류(몬스터 아이디 입력)
	DWORD		m_dwMonResNum   [MAX_CTRLDROPMOB];			//-몬스터1 발생 수량
	DWORD		m_dwMonActAttack[MAX_CTRLDROPMOB];			//-몬스터 타입1에서 선공 몬스터 발생 수량(위에 필드 수량 초과해서는 않됨)

	// 트랩관련
	DWORD		m_dwTrapOperType;              // 트랩 발동 타입 TOT_RANDOM, TOT_NOENDU
	DWORD		m_dwTrapRandomPer;             // 트랩 발동 확율(TOT_RANDOM일경우)
	DWORD		m_dwTrapDelay;				   // 트랩 발동 딜레이 0이면 즉시 발생
	
	DWORD		m_dwTrapKind	[MAX_TRAP];    // 트랩 종류 - 가상 스킬로 사용함 모든 트랩은 스킬임 (스킬프로퍼티의 ID값)
 	DWORD		m_dwTrapLevel	[MAX_TRAP];
	TCHAR		m_strLinkCtrlKey[MAX_KEY];		// 연결 컨트롤의 키 이름
	TCHAR		m_strCtrlKey[MAX_KEY];			// 컨트롤의 키 이름

#ifdef __S_DONGEON_0504
	DWORD		m_dwSetQuestNum1;			//-박스 열고난후 적용(퀘스트 넘버,퀘스트 플래그 넘버)       - UA_QUEST
	DWORD		m_dwSetFlagNum1;
	DWORD		m_dwSetQuestNum2;			//-박스 열고난후 적용(퀘스트 넘버,퀘스트 플래그 넘버)       - UA_QUEST
	DWORD		m_dwSetFlagNum2;
	DWORD 		m_dwSetItemCount;
	DWORD		m_dwTeleWorldId;
	DWORD		m_dwTeleX;
	DWORD		m_dwTeleY;
	DWORD		m_dwTeleZ;
#endif // __S_DONGEON_0504
	
} CCtrlElem, *LPCtrlElem;


#include "commonCtrl.h"

class CRespawnInfo
{
public:
	DWORD			m_dwType;
	DWORD			m_dwIndex;
	volatile long	m_cb;
	volatile long	m_nActiveAttackNum;
	CRect			m_rect;
	D3DXVECTOR3		m_vPos; // flying일 경우만 의미가있음 
	u_short			m_uTime;
	short			m_cbTime;

	u_short			m_cbRespawn;
	BOOL			m_bHalf;
	int m_nItemMin; // 아이템 최소 갯수  
	int m_nItemMax; // 아이템 최대 갯수  
	int m_nDayMin;
	int m_nDayMax;
	int m_nHourMin;
	int m_nHourMax;
	float	m_fY; // 발생할 y 좌표 
	CCtrlElem m_CtrlElem;

#ifdef __YAIMONSTER_EX
	DWORD	m_dwAiState;
	float	m_fAngle;	
#ifdef __Y_PATROL
	DWORD	m_dwPatrolIndex;
	BYTE    m_bPatrolCycle: 1;			// 전체 순환이냐? 끝->처음->끝 방향이냐	
#endif //__Y_PATROL
#endif //__YAIMONSTER_EX

	DWORD m_dwAI;		//리스폰 구역별 AI
		
public:
//	Constructions
	CRespawnInfo();
	virtual	~CRespawnInfo();
//	Operations
	CRespawnInfo&	operator=( CRespawnInfo & ri );
	void			Clear( void )	{	m_cb	= 0;	}
	u_long			Get( void )		{	return m_cb;	}
	void			Increment( BOOL bActiveAttack );	
	void			GetPos( D3DXVECTOR3 & v );
};


//#define	MAX_RESPAWN		1024
class CRespawner
{
public:
//{{AFX
private:
public:
	CRespawnInfo	m_aRespawnInfo[MAX_RESPAWN];
	int				m_nRespawnInfo;
	
//	Constructions
	CRespawner();
	virtual	~CRespawner();
//	Operations

	int				Add( CRespawnInfo & ri );
	u_long			Spawn( CWorld* pWorld );
	void			Increment( int i, BOOL bActiveAttack );
//}}AFX
};

#endif	// __RESPAWNER_H__