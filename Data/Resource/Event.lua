----------------------------------------------------------------------
---- 이벤트 관련 루아 함수 로딩 --------------------------------------
----------------------------------------------------------------------
dofile(".\\LuaFunc\\EventFunc.lua")

----------------------------------------------------------------------
-- 자동 공지 (각 서버군 TRANS Server 만 수정하면 돼요 ^^) ------------
----------------------------------------------------------------------
bNotice = false		-- 자동 공지 실행 여부(true or false)
Notice( 1 )
--{
	SetNoticeTime( "Thu 8:50", 3 , 3 )	-- 자동공지시간, 공지간격(MIN), 공지 횟수
	AddMessage( "안녕하세요. Flyff 입니다." )
	AddMessage( "잠시후 9시 부터 정기점검이 진행될 예정이오니 " )
	AddMessage( "안전한 장소에서 접속을 종료해 주시기 바랍니다." )	
--}

Notice( 2 )
--{
	SetNoticeTime( "Thu 10:00", 120, 99 )	-- 자동공지시간, 공지간격(MIN), 공지 횟수
	AddMessage( "안녕하세요. Flyff 입니다." )
	AddMessage( "타인에게 아이템을 빌려주거나 대여하는 행위는 복구 및 재제가 불가능 합니다." )
	AddMessage( "절대로 타인에게 자신의 아이템을 빌려주는 행위는 삼가해 주시기 바랍니다." )
--}

Notice( 3 )
--{
	SetNoticeTime( "Any 23:00", 1440, 99 )	-- 자동공지시간, 공지간격(MIN), 공지 횟수
	AddMessage( "안녕하세요. Flyff 입니다." )
	AddMessage( "청소년 보호 제도가 시행됨에 따라 만 16세 미만의 고객님은" )
	AddMessage( "오후 12시부터 오전 6시 까지 접속이 제한 되오니 이 점 참고하여" )
	AddMessage( "안전한 장소에서 접속을 종료해 주시기 바랍니다." )	
--}

Notice( 4 )
--{
	SetNoticeTime( "Any 23:50", 1440, 99 )	-- 자동공지시간, 공지간격(MIN), 공지 횟수
	AddMessage( "안녕하세요. Flyff 입니다." )
	AddMessage( "청소년 보호 제도가 시행됨에 따라 만 16세 미만의 고객님은" )
	AddMessage( "오후 12시부터 오전 6시 까지 접속이 제한 되오니 이 점 참고하여" )
	AddMessage( "안전한 장소에서 접속을 종료해 주시기 바랍니다." )	
--}

----------------------------------------------------------------------
---- 초기화 ----------------------------------------------------------
----------------------------------------------------------------------

-- 시간대별 아이템 드롭 가중치
tHour = { 505, 409, 324, 280, 220, 203, 202, 212,
	  227, 261, 302, 349, 571, 701, 764, 803,
	  790, 789, 754, 849, 936, 940, 919, 720 }
----------------------------------------------------------------------


----------------------------------------------------------------------------------------------------------------
----  1. AddEvent( strDesc )				-- 이벤트 추가 및 설명 등록
----  2. SetTime( strStartTime, strEndTime )	-- 해당 이벤트의 시작 시간, 종료 시간 등록(여러개 설정 가능)
----											   ( 시간 형식 -- "2007-05-03 17:53" )
----  3. SetItem( ItemId, nMax, nNum, nMinLevel, nMaxLevel )	-- 이벤트용 드롭 아이템, 일일 최대량, 드롭 갯수(랜덤),
----   								아이템을 드롭할 몬스터의 최소, 최대 레벨 - 여러개 가능
----  4. SetExpFactor( fFactor )			-- 경험지 증가 배수
----  5. SetItemDropRate( fFactor )			-- 아이템 드롭률 증가 배수
----  6. SetPieceItemDropRate( fFactor )		-- 몬스터가 가지고 있는 낱개 아이템의 드랍률 증가 배수
----  7. SetGoldDropFactor( fFactor )			-- 페냐 드롭 배수
----  8. SetAttackPower( nAttackPower )		-- 공격력 증가
----  9. SetDefensePower( nDefensePower )		-- 방어력 증가
---- 10. SetCouponEvent( SEC(n) )			-- 쿠폰 이벤트( 접속시간 - SEC(n) 또는 MIN(n) )
---- 11. SetLevelUpGift( nLevel, "all", ItemId, nNum, byFlag, nLegendClass, nJob ) -- 레벨업 선물( nLevel달성시 아이템 지급, "all" 부분에 특정 계정 지정 가능(예. "__bu" - 버디, "__an" - 엔젤 ), nJob으로 직업지정(JOB_MAX 기입시 모든 직업) 
---- 12. SetCheerExpFactor( fFactor )			-- 응원 경험치 설정
---- 13. SetSpawn( TYPE, strId, nNum, byFlag )		-- 스폰이벤트 : Type - ITEM 또는 MONSTER, ID, 1일 스폰량, 플래그값( 아이템 : 0-일반, 2-귀속 / 몬스터 : 선공여부 셋팅(0:비선공, 1:선공) 
---- 14. SetKeepConnectEvent( nTime, strItemId, nItemNum, byFlag )	-- 누적 접속 아이템 지급 이벤트( 접속시간, 아이템 ID, 아이템 개수, 플래그값:0-일반, 2-귀속 )
---- 15. SetWeatherEvent( fExpFactor, strTitle )		-- 날씨이벤트(경험치 증가 배수, 날씨 이벤트 제목) : 15차에 추가됨
---- 16. SetShopBuyFactor( fBuyFactor )		-- 상점 가격 비율 변경(유저가 사는 가격 비율 변경)
---- 17. SetShopSellFactor( fSellFactor )		-- 상점 가격 비율 변경(유저가 파는 가격 비율 변경)
---- 18. SetTroupeExpUpEvent( nTroupeMemberCount, fExpFactor )	-- 극단 사냥 시 경험치 증가 배수( 효과 적용 받기 위한 최소 극단원 수, 경험치 증가 배수 )
---- 19. SetDailyKeepConnectEvent( 요일(0 = Sunday, 1 = Monday, ..., 6 = Saturday), 접속시간, 아이템 아이디, 최소지급개수(일반모드), 최대지급개수(버닝모드), 아이템 플래그:0-일반, 2-귀속 )
---- *** 이벤트 추가 등록시 1번부터 반복하고 3~19번은 필요에 따라 생략가능하다.
----------------------------------------------------------------------------------------------------------------
--[[ SAMPLE
AddEvent( "EVENT TEST 01" )
--{
	SetTime( "2007-06-08 14:23", "2007-06-08 16:11" )
	SetTime( "2007-06-09 14:23", "2007-06-10 00:00" )
	
	SetItem( "II_SYS_SYS_EVE_HAPPYMONEY01", 30000, 5, 15, 80 )
	SetItem( "II_SYS_SYS_EVE_PUMPKIN01", 2000, 3, 81, 150 )
	SetExpFactor( 2 )
	SetItemDropRate( 2 )
	SetPieceItemDropRate( 2 )
	SetGoldDropFactor( 2 )
	SetAttackPower( 150 )
	SetDefensePower( 100 )
	SetCouponEvent( MIN(120) )
	SetLevelUpGift( 60, "all", "II_SYS_SYS_SCR_BX_PET_LAWOLF7", 1, 2, LEGEND_CLASS_NORMAL, JOB_KNIGHT )
	SetLevelUpGift( 80, "all", "II_SYS_SYS_SCR_BX_PET_LAWOLF7", 1, 2, LEGEND_CLASS_MASTER, JOB_KNIGHT_MASTER )
	SetLevelUpGift( 125, "all", "II_SYS_SYS_SCR_BX_PET_LAWOLF7", 1, 2, LEGEND_CLASS_HERO, JOB_KNIGHT_HERO)
	SetLevelUpGift( 140, "all", "II_SYS_SYS_SCR_BX_PET_LAWOLF7", 1, 2, LEGEND_CLASS_LEGEND_HERO, JOB_LORDTEMPLER_HERO )
	SetCheerExpFactor( 1.3 )
	SetSpawn( ITEM, "II_SYS_SYS_EVE_HAPPYMONEY01", 10000, 0 )
	SetSpawn( ITEM, "II_SYS_SYS_EVE_HAPPYMONEY01", 10000, 2 )
	SetSpawn( MONSTER, "MI_AIBATT1", 2000, 0 )
	SetSpawn( MONSTER, "MI_AIBATT4", 2000, 1 )
	SetKeepConnectEvent( MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 2 )
	SetWeatherEvent( 2, "비가오면 경험치가 2배~!" )
	SetShopBuyFactor( 1.5 )
	SetShopSellFactor( 0.5 )
	SetTroupeExpUpEvent( 8, 1.2 )
	SetDailyKeepConnectEvent( 0, MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 3, 2 )
	SetDailyKeepConnectEvent( 1, MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 3, 2 )
	SetDailyKeepConnectEvent( 2, MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 3, 2 )
	SetDailyKeepConnectEvent( 3, MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 3, 2 )
	SetDailyKeepConnectEvent( 4, MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 3, 2 )
	SetDailyKeepConnectEvent( 5, MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 3, 2 )
	SetDailyKeepConnectEvent( 6, MIN(60), "II_SYS_SYS_SCR_BXTREASURE01", 1, 3, 2 )
--}
--]]

-------------------------------------------------------------------------
---- Begin Script -------------------------------------------------------
-------------------------------------------------------------------------



