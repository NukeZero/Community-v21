tEventDrop = {}   -- 이벤트 저장 테이블


-- 시간대별 아이템 드롭 가중치
tHour = { 505, 409, 324, 280, 220, 203, 202, 212,
	  227, 261, 302, 349, 571, 701, 764, 803,
	  790, 789, 754, 849, 936, 940, 919, 720 }


---------------------------------------------------------------------------
------ 데이터 추가 함수 ---------------------------------------------------
---------------------------------------------------------------------------

-- 새로운 이벤트 추가
function AddEventDrop( strDesc )
	local nEventId 	= #tEventDrop + 1
	
	tEventDrop[nEventId] = {}
	tEventDrop[nEventId].tLang = {}
	tEventDrop[nEventId].tItem = {}
	tEventDrop[nEventId].tPeriod = {}
	tEventDrop[nEventId].strDesc = strDesc
	tEventDrop[nEventId].dwLevelMin = 0
	tEventDrop[nEventId].dwLevelMax = 0	
end

-- 국가 등록
function SetLang( nLang )
	local nEventId 	= #tEventDrop
	local nSize 	= #tEventDrop[nEventId].tLang + 1

	tEventDrop[nEventId].tLang[nSize] = {}
	tEventDrop[nEventId].tLang[nSize].nLang = nLang
end

-- 아이템
function SetItem( strItemId, limitQuantityPerOneDay, randomQuantityPerOneMonster )
	local nEventId 	= #tEventDrop
	local nSize 	= #tEventDrop[nEventId].tItem + 1
	
	tEventDrop[nEventId].tItem[nSize] = {}
	tEventDrop[nEventId].tItem[nSize].strItemId = strItemId
	tEventDrop[nEventId].tItem[nSize].limitQuantityPerOneDay = limitQuantityPerOneDay
	tEventDrop[nEventId].tItem[nSize].randomQuantityPerOneMonster = randomQuantityPerOneMonster
end


-- 시작시간, 끝시간
function SetTime( strStart, strEnd )
	local nEventId 	= #tEventDrop
	local nSize 	= #tEventDrop[nEventId].tPeriod + 1
	
	tEventDrop[nEventId].tPeriod[nSize] = {}
	tEventDrop[nEventId].tPeriod[nSize].strStart = strStart
	tEventDrop[nEventId].tPeriod[nSize].strEnd = strEnd
end

-- 몬스터 레벨
function SetLevel( dwLevelMin, dwLevelMax )
	local nEventId 	= #tEventDrop
	
	tEventDrop[nEventId].dwLevelMin = dwLevelMin
	tEventDrop[nEventId].dwLevelMax =dwLevelMax
end
		


