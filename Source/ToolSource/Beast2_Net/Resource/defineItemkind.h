#ifndef __DEFINE_ITEMKIND
#define __DEFINE_ITEMKIND

// IK_2는 탭의 의미적인 구분, IK_3은 아이템 생성에 사용

////////////////////////////////////////////////////////////////////////////
// 1차 구분 
//////////////////////////////////////////////////////////////////////////
#define IK1_GOLD                   0 // 돈<-
#define IK1_WEAPON                 1 // 아이템<-
#define IK1_ARMOR                  2 // 아이템<-
#define IK1_GENERAL                3 // 아이템<-
#define IK1_RIDE                   4 // 아이템<-
#define IK1_SYSTEM                 5 // <-
#define IK1_CHARGED				   6 // 상용화 아이템 여부 판단.

////////////////////////////////////////////////////////////////////////////
// 2차 구분 
//////////////////////////////////////////////////////////////////////////
#define IK2_GOLD                   0 // 돈<-
#define IK2_WEAPON_HAND            1 // 단거리<-
#define IK2_WEAPON_DIRECT          2 // 단거리<-
#define IK2_WEAPON_MAGIC           3 // 마법<-
#define IK2_ARMOR                  7 // 방어구(갑옷, 방패)<-
#define IK2_ARMORETC               8 // 주변 방어구(건틀렛 등등)<-
#define IK2_CLOTH                  9 // 의상(일반 의상들)<-
#define IK2_CLOTHETC              10 // 주변 의상(건틀렛 등등)<-
#define IK2_REFRESHER             11 // 향수 형태<-
#define IK2_POTION                12 // 물병 형태<-
#define IK2_JEWELRY               13 // 장신구<-
#define IK2_FOOD                  14 // 음식<-
#define IK2_MAGIC                 15 // 마법물품<-
#define IK2_GEM                   16 // 보석<-
#define IK2_MATERIAL              17 // 재조 재료<-
#define IK2_TOOLS                 18 // 도구<-
#define IK2_SYSTEM                19 // 시스템에 사용<-
#define IK2_RIDING                20 // 소형 비행체<-
#define IK2_MOB                   21 // <-몬스터에게 사용되는 아이템
#define IK2_BLINKWING             22 //<-
#define IK2_AIRFUEL               23 // 비행체 연료들<-
#define IK2_CHARM                 24 // 포스터
#define IK2_BULLET                25 // 발사체
#define IK2_TEXT                  26 // 문서
#define IK2_GMTEXT				  27 // GM명령
#define IK2_GENERAL				  28 // 장군아이템...이 아니고 걍 구분할거 없을때 이거 씀 -_-;;;;
#define	IK2_BUFF					29	// 버프 아이템
#define	IK2_WARP				30

////////////////////////////////////////////////////////////////////////////
// 3차 구분 
//////////////////////////////////////////////////////////////////////////
#define IK3_GOLD                   0 //<-
// 무기에 관련된 것 (IK_WEAPON 소속)
#define IK3_HAND                   1 //<-
#define IK3_SWD                    2 //<-
#define IK3_AXE                    3 //<-
#define IK3_CHEERSTICK             4 // <-
#define IK3_KNUCKLEHAMMER          5 //<-
#define IK3_WAND                   6 //<-
#define IK3_STAFF                  7 //<-
#define IK3_THSWD                  8 // 양손 소드
#define IK3_THAXE                  9 // 양손 엑스
#define IK3_VIRTUAL               10 //<-몬스터가 사용하는 가상 아이템
#define IK3_YOYO                  11 //요요
#define IK3_BOW                   12 //활
#define IK3_YOBO				  13 //요요와 활을 동시에 사용

// 방어구에 관련된 것 (IK_ARMOR 소속)
#define IK3_SHIELD                16 // 방패<-
#define IK3_HELMET                17 // 머리 보호 방어구<-
#define IK3_SUIT                  18 // 상체(통짜) 보호 방어구<-
#define IK3_GAUNTLET              19 // 손 보호 방어구<-
#define IK3_BOOTS                 20 // 발 보호 방어구<-

// 의상에 관련된 것 (IK2_CLOTH 소속)
#define IK3_HAT                   21 // 모자 의상<-
#define IK3_MASK                  22 // 신발 의상<-
#define IK3_SHOES                 23 // 신발 의상<-
#define IK3_CLOAK                 24 // 망토 의상<-
#define IK3_CLOTH                 57 // 옷
#define IK3_GLOVE                 58 // 장갑

// 일반 아이템들
#define IK3_REFRESHER             25 // 정신력 치료<-
#define IK3_POTION                26 // 각종 물병<-
#define IK3_EARRING               27 // 귀걸이<-
#define IK3_NECKLACE              28 // 목걸이<-
#define IK3_RING                  29 // 반지<-
#define IK3_INSTANT               30 // 즉석으로 먹을수 있게 만들어진 음식<-
#define IK3_COOKING               31 // 요리된 음식(음식)<-
#define IK3_ICECEARM              32 // 만들어진 아이스크림2<-
#define IK3_PILL                  59 // 영양 보조 알약
#define IK3_MAGICTRICK            33 // 각종 마법용품<-
#define IK3_GEM                   34 // 그냥 보석<-
#define IK3_DRINK                 35 // 활력제<-
#define IK3_COLLECTER		      36 // 콜렉터<-
#define IK3_ELECARD		          37 // 속성 카드<-
#define IK3_DICE		          38 // 파워 주사위<-
#define IK3_SUPSTONE		      39 // 보조석<-
 
// 탈것
#define IK3_BOARD                 40 // 보드 형태의 탈 것(손사용가능)<-
#define IK3_STICK                 41 // 올라 타는 것(손사용불능)<-
#define IK3_EVENTMAIN             42 // <-이벤트용 아이템  지급자동, 판매, 트레이드, 타임줍기 불가
#define IK3_QUEST                 43 // 퀘스트용 아이템<-
#define IK3_MAP                   44 // 퀘스트용 아이템<-
#define IK3_BLINKWING             45 // 퀘스트용 아이템<-
#define IK3_EVENTSUB              46 // <-이벤트용 아이템  지급자동, 일반아이템과 동일
#define IK3_TOWNBLINKWING         47 // <-마을로만 돌아가게 되는 블링크윙

//비행체 아이템2
#define IK3_ACCEL		          48 // 가속 연료 아이템<-
#define IK3_DELETE		          49 // <-dwItemkind3가 이것으로 설정되면 캐릭터가 로딩될 때 자동으로 지운다.

//상용화 아이템
#define IK3_SCROLL		          50 //<-
#define IK3_ENCHANTWEAPON	      51
#define IK3_CFLIGHT		          52
#define IK3_BFLIGHT		          53
#define IK3_MAGICBOTH		      54
#define IK3_KEY				      55
#define IK3_BCHARM		          55
#define IK3_RCHARM		          56 
#define IK3_ARROW		          60

//소켓 카드 와 피어싱 주사위
#define IK3_PIERDICE			  61
#define IK3_SOCKETCARD			  62

//문서 구분
#define IK3_TEXT_BOOK             70 // 문서
#define IK3_TEXT_SCROLL	          71 // 문서
#define IK3_TEXT_LETTER           72 // 문서

//운영자 명령 아이템화
#define	IK3_TEXT_UNDYING		  80
#define	IK3_TEXT_DISGUISE		  81
#define IK3_TEXT_INVISIBLE		  82
#define IK3_TEXT_GM				  83

// BINDS
#define	IK3_BINDS		84

// PET
#define IK3_PET					100		// 펫소환 아이템

#define MAX_ITEM_KIND3           119 // ItemKind3 총 갯수 
#define MAX_UNIQUE_SIZE	         400

// 컨트롤 오브젝트 Kind
#define CK1_CHEST				0    // <-보물상자
#define CK1_DOOR				1    // <-문
#define CK1_TRIGGER				2    // 동작 오브젝트

#define CK2_FADE				1    // 사라지는 컨트롤
#define CK2_KEEP				2    // 없어지지 않는 컨트롤

#define CK3_FULL				1    // 풀에니메이션
#define CK3_HALF				2    // 1/2 에니메이션

#define PIERCING_SUIT			1	// 1차 피어싱 슈트
#define PIERCING_WEAPON			2	// 2차 피어싱 무기

#endif