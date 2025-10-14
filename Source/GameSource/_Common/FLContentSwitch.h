
//date : 20101026
//author : gmpbigsun
//note : 각종 컨텐츠의 기능활성화를 담당합니다.

//모든컨텐츠는 가능한 버젼 및 Default상태를 명시해야 하고 -> SetupAllContent
//개발버젼을 위한 세팅은 SetupUserContent_DEV에서 합니다.
//배포버젼을 위한 세팅은 SetupUserContent에서 차수에 맞게 설정합니다.
//가장 마지막 라인에 컨텐츠 타입이 있습니다. 그곳에 추가하세요

#pragma  once

enum ContentType;

typedef std::map< DWORD, DWORD >			CSContainer;
typedef CSContainer::iterator		CSIter;

enum ContentState
{
	//하위비트를 사용합니다 최대 16개 버젼을 지원합니다.
	CS_ERROR = 0,
	CS_NOT_SUPPORTED = 0x0001,		//이용불가
	CS_VER1 = 0x0002,
	CS_VER2 = 0x0004,
	CS_VER3 = 0x0008,

	CS_VER_USERMAX = CS_VER3,
	CS_VER_MAX = 0xF000,
};

struct FLContentData
{
#ifdef __INTERNALSERVER
	friend class FLContentSwitch;
#endif

private:
	CSContainer m_cBaseData;
	CSContainer m_cUserData;

	BOOL IsValidUserKey( ContentType eType );
	BOOL SetupAllUserState( ContentState eState );

public:
	size_t GetSize_UserContent( ) { return m_cUserData.size(); }
	size_t GetSize_BaseContent( ) { return m_cBaseData.size(); }

	//모든 컨텐츠의 가능한 모든 Version을 등록합니다.
	BOOL RegisterBaseContent( ContentType eType, DWORD dwBitComposion, ContentState eDefaultType );

	//차수등의 현재 사용할 Version을 등록합니다.
	BOOL RegisterUserContent( ContentType eType, ContentState eState );
	
	//현재 사용중인 유저의 컨텐츠상태를 얻습니다.
	DWORD GetUserContent( ContentType eType );

	//해당 컨텐츠의 Default상태를 얻습니다.
	DWORD GetDefaultContentState( ContentType eType );

	//BaseContent에 의거 UserContent가 오류가 없는지 체크 합니다.
	BOOL CheckUserContent( );

	//reset
	void Reset( );

	//UserContent에 없는 타입을 Default상태로 삽입합니다.
	int InsertMissedUserContent( ); 
};

class  FLContentSwitch
{
public:
	static FLContentSwitch* GetSingletonPtr( );
	DWORD GetContentState( ContentType eType ) { return m_kContent.GetUserContent( eType ); }

#ifdef __INTERNALSERVER
	void SetContentState( ContentType eType, ContentState eState ) { m_kContent.m_cUserData[ eType ] = eState; }
#endif

private:
	FLContentSwitch( ) { Init(); }
	~FLContentSwitch( ) { }
	BOOL Init( );
	void SetupAllContent( );		//모든 컨텐츠 등록
	void SetupUserContent_DEV( );   // for DEV
	void SetupUserContent( const int nUserVer );		// for release

	FLContentData m_kContent;
};

typedef FLContentSwitch CS;

_inline FLContentSwitch* GetCS() { return CS::GetSingletonPtr(); }
_inline DWORD _GetContentState( ContentType eType ) { return GetCS()->GetContentState( eType ); }

//컨텐츠 타입
enum ContentType
{
	CT_OPTION_17						//VER1, VER2 : 17차 옵션 세이브 기능 추가 
	, CT_SHOP_CART_17					//VER1, VER2 : 17차 장바구니기능
	, CT_CANCELLATION					//VER1 : 17차 귀속해제
	, CT_NEW_INVENTORY					//VER1, VER2 : 17차 인벤토리확장
	, CT_COLOSSEUM						//VER1 : 17차 콜로세움
	, CT_LOADSEQ_IMAGEDATA				//VER1 : resfile을 나중에 읽음, VER2 : resfile을 먼저읽음
	, CT_ITEM_CHARGE					//VER1 : 17차 아이템 유료화	
	, CT_BARUNA_PEARCING				//VER1 : 17차 바루나 피어싱
	, CT_COSTUME_MIX					//VER1 : 17차 코스튬 합성
	, CT_DONATION						//VER1 : 17차 협동기부
	, CT_BLIND_WNDMAP					//VER1 : 전체맵, 미니맵 유료화적용
	, CT_TELEPORTER						//VER1 : 17차 텔레포트
	, CT_NEW_ENCHANTEFFECT				//VER1 : 기존 제련효과, VER2 : 신 제련효과
	, CT_RENDER_QUESTEMOTICON			//VER1 : 표시없음, VER2 : 퀘스트종류별 아이콘, 전체맵 표시
	, CT_EVENT_ARENA					//VER1: 18차 이벤트용 아레나
	, CT_INFO_NOTICE					//VER1, VER2: 공지사항 알림방식 변경
	, CT_TICKETITEM						//VER1 : 18차 유료 지도 입장 아이템
	, CT_DROP_ITEM_REFORM				//VER1 : 18차 아이템 드랍 개선
	, CT_TREASURE_CHEST					//VER1 : 보물상자 아이템
	, CT_CONSIGNMENT_MARKET				//VER1 : 중개거래소
	, CT_SERVER_MOVEMENT				//VER1 : 캐릭터 월드 이동
	, CT_ELLDINS_JAR					//VER1 : 엘딘의 항아리 ( 포션 저장소 )
	, CT_MADRIGAL_GIFT					//VER1 : 마드리갈의 선물
	, CT_SHUTDOWN_RULE					//VER1 : 셧다운 제도
	, CT_COMPOSE_TWOWEAPON19			//VER1 : 양손무기합성
	, CT_NEWUI_19						//VER1, VER2 : UI 전면수정
	, CT_LOOKS_CHANGE					//VER1 : 아이템 외형 변경
	, CT_DB_ITEM_EXTEND					//VER1 : DB 아이템 관련 테이블 확장
	, CT_ATTR_SYSTEM_MODIFY				//VER1 : 속성 제련 관련 보정 수치 수정
	, CT_REAWAKENING					//VER1 : 각성 재굴림
	, CT_FLYFF_PIECE_SYSTEM				//VER1 : 프리프 조각 시스템
	

	, CT_MAX
};





