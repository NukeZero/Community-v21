#ifndef __VERSION_COMMON_H__
#define __VERSION_COMMON_H__

#define __ERRORTXT

// exception handler관련
#define __PATCH_CLIENT		// COption클래스만을 단독 사용한다.

#define LANG_KOR 0
#define LANG_ENG 1
#define LANG_JAP 2
#define LANG_CHI 3
#define LANG_THA 4
#define LANG_TWN 5
#define LANG_GER 6
#define LANG_SPA 7
#define LANG_FRA 8
#define LANG_POR 9
#define LANG_VTN 10
#define LANG_RUS 11

#define	LANG_FWC 19

// 국가 정의를 언어권 정의와 값이 겹치지 않도록 한다
#define CNTRY_USA 20
#define CNTRY_PHI 21
#define CNTRY_TWN 22
#define CNTRY_HK  23
#define CNTRY_IND 24

#define	CNTRY_FWC 30

#define __YENV
#define __SFX_OPT

// 필요에 따라서 주석문을 풀고 컴파일 해주세요.
// 컴파일이 끝나면 undo check out하시면 됩니다.

//#define __MAINSERVER						// 국내 정섭
//#define __TESTSERVER_EXTERNAL				// 외부 테섭
//#define __TESTSERVER_INTERNAL_CURR_VER		// QA팀 현재 차수 테섭
//#define __TESTSERVER_INTERNAL_NEXT_VER	// QA팀 다음 차수 테섭
//#define __TESTSERVER_FOREIGN_17			//  17차 해외 QA서버
//#define __TESTSERVER_KOREA_16				//  16차 국내 QA서버

//#define __TESTSERVER_FOREIGN	// 해외에서 접속하는 사내 테스트 서버



//#define __LANG_JAP
	//#define __JAPAN
	//#define __JAPAN_NEW_TEST
	//#define __JP_TEST2
//#define __LANG_THAI
	//#define __LANG_THAI_MAIN
	//#define __LANG_THAI_TEST
//#define __LANG_TAIWAN
	//#define	__LANG_TAIWAN_MAIN				// Main 15차 
	//#define	__LANG_TAIWAN_MAIN_NEXT			// Main 16차
	//#define	__LANG_TAIWAN_TEST				// Test 15차 	
	//#define	__LANG_TAIWAN_TEST_NEXT			// Test 16차 
	//#define	__LANG_TAIWAN_TEST3				// Test 17차 
	//#define	__LANG_TAIWAN_HONGKONG
	//#define	__LANG_TAIWAN_HONGKONG_TEST
//#define __LANG_ENG
	//#define __LANG_ENG_PHP
	//#define __LANG_ENG_PHPTEST
	//#define __LANG_ENG_USA
	//#define __LANG_ENG_USATEST
	//#define __LANG_ENG_ID_MAIN
	//#define __LANG_ENG_ID_TEST
	//#define __LANG_ENG_ID_TEST2
//#define __LANG_GER
	//#define __LANG_GER_MAIN			// 독일 정식 서버
	//#define __LANG_GER_TEST			// 독일 16차 테스트 서버
	//#define __LANG_GER_NEXT_TEST		// 독일 15차 테스트 서버
//#define __LANG_SPA
	//#define __LANG_SPA_CHIL			//칠레
	//#define __LANG_SPA_CHILTEST
//#define __LANG_FRA
	//#define __LANG_FRA_FRA			// 프랑스 정식 서버
	//#define __LANG_FRA_TEST			// 프랑스 16차 테스트 서버
	//#define __LANG_FRA_NEXT_TEST		// 프랑스 15차 테스트 서버
//#define __LANG_POR
	//#define __LANG_POR_BRA
	//#define __LANG_POR_BRATEST
	//#define __LANG_POR_BRATEST2
	//#define __LANG_POR_BRATEST3
//#define __LANG_VTN
	//#define __LANG_VTN_MAIN
	//#define __LANG_VTN_TEST
//#define __LANG_RUS
	//#define __LANG_RUS_MAIN
	//#define __LANG_RUS_TEST
//#define __LANG_FWC					// FWC
	//#define __LANG_FWC_TAIWAN
//	#define __LANG_FWC_EUROPE
//	#define __LANG_FWC_USA
//	#define __LANG_FWC_CHILE_SPANISH
//	#define __LANG_FWC_THAILAND
//	#define __LANG_FWC_JAPAN
//	#define __LANG_FWC_BRAZIL
//	#define __LANG_FWC_INDONESIA
//	#define __LANG_FWC_KOREA
//	#define	__LANG_FWC_KOREA_LOCAL

#if	  defined(__TESTSERVER_INTERNAL_CURR_VER)		// QA 현재 차수 테섭
	#define	__VER 9
	#define __CURRENT_LANG LANG_KOR
	const int PATCH_VERSION = 49;
	#define WEB_FIRST_PAGE	"http://notice.flyff.com/news/news_list_dev.asp" //임시로 정섭 공지사항 링크 사용
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define SERVER_ROOT		"ResClient"
	#define PATCHSEVER_URL	"172.16.100.16"
	#define	HOME_LINK		"http://www.flyff.com"
	#define VERSION_NAME	"Kor"
	#define LOGIN_LINK		"http://test.flyff.com"
	#define __LINK_PORTAL

#elif defined(__TESTSERVER_INTERNAL_NEXT_VER)		// QA 다음 차수 테섭
	#define	__VER 9
	#define __CURRENT_LANG LANG_KOR
	const int PATCH_VERSION = 45;
	#define WEB_FIRST_PAGE	"http://notice.flyff.com/news/news_list_dev.asp" //임시로 정섭 공지사항 링크 사용
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define SERVER_ROOT		"ResClient"
	#define PATCHSEVER_URL	"flyfftest2"
	#define	HOME_LINK		"http://www.flyff.com"
	#define VERSION_NAME	"Kor"
	#define LOGIN_LINK		"http://test.flyff.com"
	#define __LINK_PORTAL

#elif defined(__TESTSERVER_EXTERNAL)		// 외부 테섭
	#define	__VER 10
	#define __CURRENT_LANG LANG_KOR
	const int PATCH_VERSION = 51;
	#define WEB_FIRST_PAGE	"http://notice.flyff.com/news/news_list_dev.asp" //임시로 정섭 공지사항 링크 사용
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define SERVER_ROOT		"DOWN/PATCH/ResClient_test"
	#define PATCHSEVER_URL	"patch.flyff.gpotato.kr"
	#define	HOME_LINK		"http://www.flyff.com"
	#define VERSION_NAME	"Kor"
	#define LOGIN_LINK		"http://test.flyff.com"
	#define __LINK_PORTAL

#elif defined(__MAINSERVER)		// 외부 본섭
	#define	__VER 10
	#define __CURRENT_LANG LANG_KOR
	const int PATCH_VERSION = 51;
//	#define WEB_FIRST_PAGE	"http://notice.flyff.com/news/news_list_dev.asp"
	#define WEB_FIRST_PAGE	"http://en.flyff.gpotato.com/common/aspx/LatestNews.aspx"
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define SERVER_ROOT		"DOWN/PATCH/ResClient"
	#define PATCHSEVER_URL	"patch.flyff.gpotato.kr"
	#define	HOME_LINK		"http://www.flyff.com"
	#define VERSION_NAME	"Kor"
	#define LOGIN_LINK		"http://www.flyff.com"
	#define __LINK_PORTAL

#elif defined(__TESTSERVER_FOREIGN_17)		//  17차 해외 QA서버
	#define	__VER 5
	#define __CURRENT_LANG LANG_KOR
	const int PATCH_VERSION =49;
	#define WEB_FIRST_PAGE	"" //임시로 정섭 공지사항 링크 사용
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define SERVER_ROOT		"ResClient"
	#define PATCHSEVER_URL	"172.16.100.21"
	#define	HOME_LINK		""
	#define VERSION_NAME	""
	#define __LINK_PORTAL

#elif defined(__TESTSERVER_KOREA_16)		//  16차 국내 QA서버
	#define	__VER 5
	#define __CURRENT_LANG LANG_KOR
	const int PATCH_VERSION = 49;
	#define WEB_FIRST_PAGE	"" //임시로 정섭 공지사항 링크 사용
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define SERVER_ROOT		"ResClient"
	#define PATCHSEVER_URL	"172.16.100.20"
	#define	HOME_LINK		""
	#define VERSION_NAME	""
	#define __LINK_PORTAL
	
#elif defined(__TESTSERVER_FOREIGN)		// 해외에서 접속하는 사내 테스트 서버
	#define	__VER 9
	#define __CURRENT_LANG LANG_KOR
	const int PATCH_VERSION = 49;
	#define WEB_FIRST_PAGE	""
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define SERVER_ROOT		"ResClient"
	#define PATCHSEVER_URL	"172.16.100.17"
	#define	HOME_LINK		""
	#define VERSION_NAME	""


#elif defined(__LANG_JAP)		// 일본
	#if defined(__JAPAN)
		#define	__VER 9
		#define __CURRENT_LANG LANG_JAP
		#define __HANGAME0307
		const int PATCH_VERSION = 52;
		#define WEB_FIRST_PAGE	"http://flyff.gpotato.jp/launcher/index.aspx"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define PATCHSEVER_URL	"cdn.gpotato.jp"
		#define SERVER_ROOT		"flyff/patch_fl"
		#define	HOME_LINK		"http://www.flyff.jp"
		#define VERSION_NAME	"Japan"
	#elif defined(__JAPAN_NEW_TEST)
		#define	__VER 9
		#define __CURRENT_LANG LANG_JAP
		#define __HANGAME0307
		const int PATCH_VERSION = 51;
		#define WEB_FIRST_PAGE	"http://test.flyff.gpotato.jp/launcher/index.aspx"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define PATCHSEVER_URL	"122.211.251.204"
		#define SERVER_ROOT		"ResClient"
		#define	HOME_LINK		"http://www.flyff.jp"
		#define VERSION_NAME	"Japan"
	#elif defined(__JP_TEST2)
		#define	__VER 9
		#define __CURRENT_LANG LANG_JAP
		#define __HANGAME0307
		const int PATCH_VERSION = 51;
		#define WEB_FIRST_PAGE	"http://test.flyff.gpotato.jp/launcher/index.aspx"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define PATCHSEVER_URL	"122.211.251.210"
		#define SERVER_ROOT		"ResClient"
		#define	HOME_LINK		"http://www.flyff.jp"
		#define VERSION_NAME	"Japan"
	#endif

#elif defined(__LANG_THAI)		// 태국
	#define	__VER 9
	#define __CURRENT_LANG LANG_THA
	#if defined( __LANG_THAI_MAIN )
		const int PATCH_VERSION = 52;
		#define WEB_FIRST_PAGE	"http://www.flyffonline.in.th/update_patch.html"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT		"NEUROSPACE/RES_CLIENT_THA"
		#define PATCHSEVER_URL	"patch.flyffonline.in.th"		// THAI
		#define	HOME_LINK		"http://flyffonline.ini3.co.th"
		#define VERSION_NAME	"Thai"
	#elif defined( __LANG_THAI_TEST )
		const int PATCH_VERSION = 52;
		#define WEB_FIRST_PAGE	"http://www.flyffonline.in.th/update_patch.html"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT		"NeuroSpace/RESCLIENT"
		#define PATCHSEVER_URL	"flyfftest.flyffonline.in.th"	// TEST_THAI
		#define	HOME_LINK		"http://flyffonline.ini3.co.th"
		#define VERSION_NAME	"Thai"
	#endif

#elif defined(__LANG_TAIWAN)
	#define __CURRENT_LANG LANG_TWN

	#if defined(__LANG_TAIWAN_MAIN)
    #define __CURRENT_CNTRY CNTRY_TWN
		#define	__VER 12
		const int PATCH_VERSION = 37;
		#define SERVER_ROOT         "Flyff/Neurospace/Resclient"
		#define PATCHSEVER_URL      "patch.omg.com.tw"
		#define WEB_FIRST_PAGE      "http://www.omg.com.tw/fff"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define HOME_LINK           ""//"http://www.omg.com.tw/"
		#define VERSION_NAME	    "Taiwan"
	#elif defined(__LANG_TAIWAN_TEST)									
	#define __CURRENT_CNTRY CNTRY_TWN
		#define	__VER 12
		const int PATCH_VERSION = 37;
		#define SERVER_ROOT         "Flyff/Neurospace/Resclient"
		#define PATCHSEVER_URL		"testpatch.omg.com.tw"		 
		#define WEB_FIRST_PAGE      "http://www.omg.com.tw/fff"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define HOME_LINK           ""//"http://www.omg.com.tw/"
		#define VERSION_NAME	    "Taiwan"
	#elif defined(__LANG_TAIWAN_MAIN_NEXT)									
		#define __CURRENT_CNTRY CNTRY_TWN
		#define	__VER 12
		const int PATCH_VERSION = 38;
		#define SERVER_ROOT         "Flyff/Neurospace/Resclient"
		#define PATCHSEVER_URL		"flyffpatch.omg.com.tw"		  
		#define WEB_FIRST_PAGE      "http://www.omg.com.tw/fff"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define HOME_LINK           ""//"http://www.omg.com.tw/"
		#define VERSION_NAME	    "Taiwan"
	#elif defined(__LANG_TAIWAN_TEST_NEXT)							
		#define __CURRENT_CNTRY CNTRY_TWN
		#define	__VER 12
		const int PATCH_VERSION = 38;
		#define SERVER_ROOT         "Flyff/Neurospace/Resclient"
//		#define PATCHSEVER_URL      "test2patch.omg.com.tw"
		#define PATCHSEVER_URL      "210.242.174.17"
		#define WEB_FIRST_PAGE      "http://www.omg.com.tw/fff"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define HOME_LINK           ""//"http://www.omg.com.tw/"
		#define VERSION_NAME	    "Taiwan"
	#elif defined(__LANG_TAIWAN_TEST3)							
		#define __CURRENT_CNTRY CNTRY_TWN
		#define	__VER 12
		const int PATCH_VERSION = 38;
		#define SERVER_ROOT         "Flyff/Neurospace/Resclient"
		#define PATCHSEVER_URL      "test3patch.omg.com.tw"
		#define WEB_FIRST_PAGE      "http://www.omg.com.tw/fff"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define HOME_LINK           ""//http://www.omg.com.tw/
		#define VERSION_NAME	    "Taiwan"	

#elif defined(__LANG_TAIWAN_HONGKONG)		// 홍콩
		#define __CURRENT_CNTRY CNTRY_HK
		#define	__VER 9
		const int PATCH_VERSION = 9;
		#define SERVER_ROOT         "Neurospace/Resclient"
		#define PATCHSEVER_URL      "patch.flyff.gamecyber.cc"
		#define WEB_FIRST_PAGE      "http://flyff.gamecyber.net/launcher/index.html"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define HOME_LINK           "http://flyff.gamecyber.net"
		#define VERSION_NAME	    "Hongkong"
	#elif defined(__LANG_TAIWAN_HONGKONG_TEST)	// 홍콩 테섭
		#define __CURRENT_CNTRY CNTRY_HK
		#define	__VER 9
		const int PATCH_VERSION = 9;
		#define SERVER_ROOT         "resclient"
		#define PATCHSEVER_URL      "59.152.240.193"
		#define WEB_FIRST_PAGE      "http://flyff.gamecyber.net/launcher/index.html"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define HOME_LINK           "http://flyff.gamecyber.net"
		#define VERSION_NAME	    "Hongkong"
	#endif

#elif defined(__LANG_ENG)
	#define __CURRENT_LANG LANG_ENG

	#if defined(__LANG_ENG_PHP)
		#define __COUNTRY CNTRY_PHI
		#define	__VER 9
		const int PATCH_VERSION = 27;  
		#define	WEB_FIRST_PAGE	"http://flyff.levelupgames.ph/news/flyffnews"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/VERSION6"
		#define	PATCHSEVER_URL	"patch.flyff.com.ph"
		#define	HOME_LINK		"http://flyff.levelupgames.ph"


		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"


		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_PHPTEST)
		#define __COUNTRY CNTRY_PHI
		#define	__VER 9
		const int PATCH_VERSION = 27;  
		#define WEB_FIRST_PAGE  "http://flyff.levelupgames.ph/news/flyffnews"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/RESCLIENT_TEST"
		#define PATCHSEVER_URL  "flyfftest.flyff.com.ph"
		#define HOME_LINK       "http://flyff.levelupgames.ph"
		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_USA)
		#define __COUNTRY CNTRY_USA
		#define	__VER 9
		const int PATCH_VERSION = 49;
		#define WEB_FIRST_PAGE	"http://en.flyff.webzen.com/common/aspx/LatestNews.aspx"
		#define WEB_BANNER_PAGE	"http://en.flyff.webzen.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT		"NeuroSpace/RESCLIENT"
		#define PATCHSEVER_URL  "us.patch.flyff.gpotato.com"
		#define HOME_LINK       "http://flyff.webzen.com"
		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_USATEST)
		#define __COUNTRY CNTRY_USA
		#define	__VER 9
		const int PATCH_VERSION = 49;
		#define WEB_FIRST_PAGE  "http://en.flyff.webzen.com/common/aspx/LatestNews.aspx"
		#define WEB_BANNER_PAGE	"http://en.flyff.webzen.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT     "NeuroSpace/RESCLIENT_TEST"
		#define PATCHSEVER_URL  "211.43.158.145"
		#define HOME_LINK       "http://flyff.webzen.com/"
		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_EU_MAIN)
		#define __COUNTRY CNTRY_USA
		#define	__VER 9
		const int PATCH_VERSION = 49;
		#define WEB_FIRST_PAGE	"http://en.flyff.webzen.com/common/aspx/LatestNews.aspx"
		#define WEB_BANNER_PAGE	"http://en.flyff.webzen.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT		"FlyffEN/RESCLIENT"
		#define PATCHSEVER_URL  "patch.flyff.gpotato.eu"
		#define HOME_LINK       "http://flyff.webzen.com"
		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_EU_TEST)
		#define __COUNTRY CNTRY_USA
		#define	__VER 9
		const int PATCH_VERSION = 49;
		#define WEB_FIRST_PAGE	"http://en.flyff.webzen.com/common/aspx/LatestNews.aspx"
		#define WEB_BANNER_PAGE	"http://en.flyff.webzen.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT		"FlyffEN/RESCLIENT"
		#define PATCHSEVER_URL  "211.43.158.143"	//EU
		#define HOME_LINK       "http://flyff.webzen.com"
		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_ID_MAIN)
		#define __COUNTRY CNTRY_IND
		#define	__VER 9
		const int PATCH_VERSION = 17;
		#define WEB_FIRST_PAGE  "http://flyff.playmojo.com/launcher/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT     "Neurospace/ResClient"
		#define PATCHSEVER_URL  "202.59.255.20"
		#define HOME_LINK       "http://flyff.playmojo.com"
		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_ID_TEST)
		#define __COUNTRY CNTRY_IND
		#define	__VER 9
		const int PATCH_VERSION = 17;
		#define WEB_FIRST_PAGE  "http://flyff.playmojo.com/launcher/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT     "Neurospace/ResClient"
		#define PATCHSEVER_URL  "202.59.255.24"
		#define HOME_LINK       "http://flyff.playmojo.com"
		#define VERSION_NAME	""
	#elif defined(__LANG_ENG_ID_TEST2)
		#define __COUNTRY CNTRY_IND
		#define	__VER 9
		const int PATCH_VERSION = 17;
		#define WEB_FIRST_PAGE  "http://flyff.playmojo.com/launcher/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT     "Neurospace/ResClient"
		#define PATCHSEVER_URL  "202.59.255.10"
		#define HOME_LINK       "http://flyff.playmojo.com"
		#define VERSION_NAME	""
	#endif

#elif defined(__LANG_GER)		// 독일
	#if defined(__LANG_GER_MAIN)
		#define	__VER 12
		#define __CURRENT_LANG LANG_GER
		const int PATCH_VERSION = 49;
		#define WEB_FIRST_PAGE      "http://de.flyff.gpotato.eu/Game/Launcher/" //"http://flyff.gpotato.eu/news"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT         "FlyffDE/RESCLIENT"
		#define PATCHSEVER_URL      "patch.flyff.gpotato.eu" //"flyffserv.gpotato.eu"
		#define HOME_LINK           "http://flyff.gpotato.eu"
		#define VERSION_NAME	    ""
	#elif defined(__LANG_GER_TEST)
		#define	__VER 12		//Image 배경이 검은색이라 Text색상 변경하기 위해 12차 버전으로 구분(독일, 프랑스)
		#define __CURRENT_LANG LANG_GER
		const int PATCH_VERSION = 49;
		#define WEB_FIRST_PAGE      "http://de.flyff.gpotato.eu/Game/Launcher/" //"http://flyff.gpotato.eu/news"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT         "FlyffDE/RESCLIENT_TEST"
		#define PATCHSEVER_URL      "211.43.158.146"
		#define HOME_LINK           "http://flyff.gpotato.eu"
		#define VERSION_NAME	    ""
	#elif defined(__LANG_GER_NEXT_TEST)
		#define	__VER 12		//Image 배경이 검은색이라 Text색상 변경하기 위해 12차 버전으로 구분(독일, 프랑스)
		#define __CURRENT_LANG LANG_GER
		const int PATCH_VERSION = 49;
		#define WEB_FIRST_PAGE      "http://de.flyff.gpotato.eu/Game/Launcher/" //"http://flyff.gpotato.eu/news"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define SERVER_ROOT         "FlyffDE/RESCLIENT_TEST"
		#define PATCHSEVER_URL      "87.198.181.190"
		#define HOME_LINK           "http://flyff.gpotato.eu"
		#define VERSION_NAME	    ""
	#endif

#elif defined(__LANG_SPA)		// 칠레
	#define	__VER 9
	#define __CURRENT_LANG LANG_SPA

	#if defined(__LANG_SPA_CHIL)
		const int PATCH_VERSION = 28;
		#define	WEB_FIRST_PAGE	"http://flyff.es.gpotato.com/news/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/ResClient"
		#define	PATCHSEVER_URL	"es.patch.flyff.gpotato.com" //"http://flyff-ftp.zoomby.net" //"200.74.164.27"
		#define	HOME_LINK		"http://flyff.es.gpotato.com"
		#define VERSION_NAME	""
	#elif defined(__LANG_SPA_CHILTEST)
		const int PATCH_VERSION = 28;
		#define	WEB_FIRST_PAGE	"http://flyff.es.gpotato.com/news/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/ResClient"
		#define	PATCHSEVER_URL	"test.es.patch.flyff.gpotato.com"
		#define	HOME_LINK		"http://flyff.es.gpotato.com"
		#define VERSION_NAME	""
	#endif

#elif defined(__LANG_FRA)		// 프랑스
	#define __CURRENT_LANG LANG_FRA

	#if defined(__LANG_FRA_FRA) //프랑스 -> BetaPatchClient.rc에서 버튼이미지 수동으로 풀어야 함.
		#define	__VER 12		//Image 배경이 검은색이라 Text색상 변경하기 위해 12차 버전으로 구분(독일, 프랑스)
		const int PATCH_VERSION = 44;
		#define	WEB_FIRST_PAGE	"http://fr.flyff.gpotato.eu/Game/Launcher/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"FlyffFR/RESCLIENT"
		#define	PATCHSEVER_URL	"patch.flyff.gpotato.eu" //"flyffserv.gpotato.eu"
		#define	HOME_LINK		"http://flyff.gpotato.eu"
		#define VERSION_NAME	""
	#elif defined(__LANG_FRA_TEST)
		#define	__VER 12
		const int PATCH_VERSION = 44;  
		#define	WEB_FIRST_PAGE	"http://fr.flyff.gpotato.eu/Game/Launcher/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"FlyffFR/RESCLIENT"
		#define	PATCHSEVER_URL	"211.43.158.144"
		#define	HOME_LINK		"http://flyff.gpotato.eu"
		#define VERSION_NAME	""
	#elif defined(__LANG_FRA_NEXT_TEST)
		#define	__VER 12
		const int PATCH_VERSION = 44;  
		#define	WEB_FIRST_PAGE	"http://fr.flyff.gpotato.eu/Game/Launcher/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"FlyffFR/RESCLIENT"
		#define	PATCHSEVER_URL	"87.198.181.191"
		#define	HOME_LINK		"http://flyff.gpotato.eu"
		#define VERSION_NAME	""
	#endif

#elif defined(__LANG_POR)		// 브라질
	#define	__VER 12
	#define __CURRENT_LANG LANG_POR

	#if defined(__LANG_POR_BRA)
		const int PATCH_VERSION = 25;
		#define	WEB_FIRST_PAGE	"http://flyff.gpotato.com.br/news/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/RESCLIENT"
		#define	PATCHSEVER_URL	"flyff-down.gpotato.com.br"
		#define	HOME_LINK		"http://flyff.gpotato.com.br"
		#define VERSION_NAME	""
	#elif defined(__LANG_POR_BRATEST)
		const int PATCH_VERSION = 25;
		#define	WEB_FIRST_PAGE	"http://flyff.gpotato.com.br/news/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/RESCLIENT_TEST"
		#define	PATCHSEVER_URL	"10.4.113.115"
		#define	HOME_LINK		"http://flyff.gpotato.com.br"
		#define VERSION_NAME	""
	#elif defined(__LANG_POR_BRATEST2)
		const int PATCH_VERSION = 25;
		#define	WEB_FIRST_PAGE	"http://flyff.gpotato.com.br/news/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/RESCLIENT_TEST"
		#define	PATCHSEVER_URL	"10.4.113.116"
		#define	HOME_LINK		"http://flyff.gpotato.com.br"
		#define VERSION_NAME	""
	#elif defined(__LANG_POR_BRATEST3)
		const int PATCH_VERSION = 25;
		#define	WEB_FIRST_PAGE	"http://flyff.gpotato.com.br/news/"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"NeuroSpace/RESCLIENT_TEST"
		#define	PATCHSEVER_URL	"200.98.210.244"
		#define	HOME_LINK		"http://flyff.gpotato.com.br"
		#define VERSION_NAME	""
	#endif
#elif defined(__LANG_VTN)		// 베트남
	#define	__VER 12
	#define __CURRENT_LANG LANG_VTN

	#if defined(__LANG_VTN_MAIN)
		const int PATCH_VERSION = 6;
		#define	WEB_FIRST_PAGE	"http://patch.flyff.vn"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"Neurospace/resclient"
		#define	PATCHSEVER_URL	"update.flyff.vn"
		#define	HOME_LINK		"http://www.flyff.vn"
		#define VERSION_NAME	""
	#elif defined(__LANG_VTN_TEST)
		const int PATCH_VERSION = 6;
		#define	WEB_FIRST_PAGE	"http://patch.flyff.vn"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT		"Neurospace/resclient_test"
		#define	PATCHSEVER_URL	"update1.flyff.vn"
		#define	HOME_LINK		"http://www.flyff.vn"
		#define VERSION_NAME	""
	#endif

#elif defined(__LANG_RUS)		// 러시아
	#define	__VER 12
	#define __CURRENT_LANG LANG_RUS

	#if defined(__LANG_RUS_MAIN)
		const int PATCH_VERSION = 22;
		#define	WEB_FIRST_PAGE		"http://flyffgame.ru/client/news"
		#define	WEB_FIRST_PAGE_2	"http://flyff.mail.ru/client/news"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT			"Neurospace/ResClient"
//		#define	PATCHSEVER_URL		"188.93.59.98" //"patch.flyffgame.ru"
		#define	PATCHSEVER_URL		"patch.flyffgame.ru"
		#define	HOME_LINK			"http://flyff.mail.ru"
		#define HOME_LINK_2			"http://flyff.mail.ru"
		#define VERSION_NAME		""
	#elif defined(__LANG_RUS_TEST)
		const int PATCH_VERSION = 22;
		#define	WEB_FIRST_PAGE		"http://flyffgame.ru/client/news"
		#define	WEB_FIRST_PAGE_2	"http://flyff.mail.ru/client/news"
		#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
		#define	SERVER_ROOT			"Neurospace/ResClient"
		#define	PATCHSEVER_URL		"188.93.59.104"
		#define	HOME_LINK			"http://flyff.mail.ru"
		#define HOME_LINK_2			"http://flyff.mail.ru"
		#define VERSION_NAME		""
	#endif

#elif defined(__LANG_FWC)
	#define	__VER			12
	#define __CURRENT_LANG	LANG_FWC
	#define __COUNTRY		CNTRY_FWC
	
	const int PATCH_VERSION = 12;
	//#define VERSION_NAME	"FWC"
	#define WEB_FIRST_PAGE  "http://fwc.flyff.com/game_news/default.aspx"
	#define WEB_BANNER_PAGE	"http://en.flyff.gpotato.com/common/aspx/mainBanner.aspx"
	#define HOME_LINK       "http://fwc.flyff.com/"

	#if defined(__LANG_FWC_TAIWAN)	// 대만
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Taiwan"
		//#define	WEB_FIRST_PAGE	"http://www.omg.com.tw/fff"
		//#define	HOME_LINK		"http://www.omg.com.tw/"
		#define	PATCHSEVER_URL	"flyffpatch.omg.com.tw"
		#define	SERVER_ROOT		"FWC/Resclient"
	#elif defined(__LANG_FWC_EUROPE)	// 유럽(독일, 프랑스)
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Europe"
		//#define WEB_FIRST_PAGE  "http://de.flyff.gpotato.eu/Game/Launcher/"
		//#define HOME_LINK       "http://flyff.gpotato.eu"
		#define PATCHSEVER_URL  "patch.flyff.gpotato.eu"
		#define	SERVER_ROOT		"FlyffFWC"
	#elif defined(__LANG_FWC_USA)	// 미국
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"United States of America"
		//#define WEB_FIRST_PAGE  "http://flyff.gpotato.com/news/"
		//#define HOME_LINK       "http://flyff.gpotato.com"
		#define PATCHSEVER_URL  "fwc.patch.download.flyff.gpotato.com"
		#define SERVER_ROOT     "ResClient"
	#elif defined(__LANG_FWC_CHILE_SPANISH)	// 칠레(스패니쉬)
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Chile(Spanish)"
		//#define WEB_FIRST_PAGE  "http://flyff.es.gpotato.com/news/"
		//#define HOME_LINK       "http://flyff.es.gpotato.com"
		#define PATCHSEVER_URL  "fwc.patch.download.flyff.es.gpotato.com"
		#define SERVER_ROOT     "ES_ResClient"
	#elif defined(__LANG_FWC_THAILAND)	// 태국
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Thailand"
		//#define WEB_FIRST_PAGE  "http://www.flyffonline.in.th/update_patch.html"
		//#define HOME_LINK       "http://flyffonline.ini3.co.th"
		#define PATCHSEVER_URL  "203.195.98.206"
		#define SERVER_ROOT     "ResClient"
	#elif defined(__LANG_FWC_JAPAN)	// 일본
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Japan"
		//#define WEB_FIRST_PAGE  "http://flyff.gpotato.jp/launcher/index.aspx"
		//#define HOME_LINK       "http://www.flyff.jp"
		#define PATCHSEVER_URL  "203.216.218.74"
		#define SERVER_ROOT     "ResClient"
	#elif defined(__LANG_FWC_BRAZIL)	// 브라질
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Brazil"
		//#define WEB_FIRST_PAGE  "http://flyff.gpotato.com.br/news/"
		//#define HOME_LINK       "http://flyff.gpotato.com.br"
		#define PATCHSEVER_URL  "200.98.210.242"
		#define SERVER_ROOT     "ResClient"
	#elif defined(__LANG_FWC_INDONESIA)	// 인도네시아
//		const int PATCH_VERSION = 17;
		#define VERSION_NAME	"Indonesia"
//		#define WEB_FIRST_PAGE  "http://flyff.playmojo.com/launcher/"
//		#define HOME_LINK       "http://flyff.playmojo.com"
		#define PATCHSEVER_URL  "202.59.255.8"
		#define SERVER_ROOT     "Neurospace/ResClient"
	#elif defined(__LANG_FWC_KOREA)	// 한국
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Korea"
		//#define WEB_FIRST_PAGE  "http://notice.flyff.com/news/news_list_dev.asp"
		//#define HOME_LINK       "http://www.flyff.com"
		#define PATCHSEVER_URL  "119.207.77.37"
		#define SERVER_ROOT     "ResClient"
	#elif defined(__LANG_FWC_KOREA_LOCAL)	// 한국 예선
		//const int PATCH_VERSION = 1;
		#define VERSION_NAME	"Korea Preliminary"
		//#define WEB_FIRST_PAGE  "http://notice.flyff.com/news/news_list_dev.asp"
		//#define HOME_LINK       "http://www.flyff.com"
		#define PATCHSEVER_URL  "119.207.77.40"
		#define SERVER_ROOT     "ResClient"

	#endif

#else

//	#error COMPILE TARGET MUST BE DEFINED.

#endif	// end - 서버종류별 define

#endif // VERSION_COMMON_H
