include( "lang.h" )

LANG_COMMON = -1
currentLanguage = LANG_COMMON
systemLanguage = GetLanguage()
tNPC = {}

tTime = {}
tPrize = {}
tMiniGamePrize = {}


function MIN( nMin )
	return nMin * 60 * 1000
end

function SetLanguage( nLanguage )
	currentLanguage = nLanguage
	tTime = {}
	tPrize = {}
	tMiniGamePrize = {}
end

function IsLoadable()
	if( currentLanguage == LANG_COMMON or currentLanguage == systemLanguage ) then
		return true;
	end
	return false;
end

function SetTime( strWeekDay, strStartTime, strEndTime, strOpenTime, nPayPenya )
	
	if( IsLoadable() == false ) then
		return
	end
		
	local strTemp
	local tTemp = {}
	
	strTemp = ""
	for i in string.gmatch( strStartTime, "%d+" ) do
		strTemp = strTemp..i
	end
	tTemp.nStartTime = tonumber(strTemp)
	
	strTemp = ""
	for i in string.gmatch( strEndTime, "%d+" ) do
		strTemp = strTemp..i
	end
	tTemp.nEndTime = tonumber(strTemp)

	strTemp = ""
	for i in string.gmatch( strOpenTime, "%d+" ) do
		strTemp = strTemp..i
	end
	tTemp.nOpenTime = tonumber(strTemp)
	
	tTemp.nPayPenya = nPayPenya
	
	tTime[strWeekDay] = tTemp
end		
	
	
function IsApplicationTime()
	local nNowTime = tonumber( os.date( "%H%M" ) )
	local tTemp = tTime[os.date("%a")]
	
	if( tTemp ~= nil ) then 
		if( (nNowTime >= tTemp.nStartTime) and (nNowTime < tTemp.nEndTime) ) then
			return true
		end
	end
	
	return false
end

function IsOpenTime()
	local nNowTime = tonumber( os.date( "%H%M" ) )
	local tTemp = tTime[os.date("%a")]
	
	if( tTemp ~= nil ) then 
		if( nNowTime == tTemp.nOpenTime ) then
			return true
		end
	end
	
	return false;
end

function GetPayPenya()
	local tTemp = tTime[os.date("%a")]
	if( tTemp ~= nil ) then
		return tTemp.nPayPenya
	end
	
	return 1000000
end

function SetNPC( strNPCId, strCharKey, xPos, yPos, zPos )
	local nIndex = #tNPC + 1
	
	tNPC[nIndex] = {}
	tNPC[nIndex].strNPCId = strNPCId
	tNPC[nIndex].strCharKey = strCharKey
	tNPC[nIndex].xPos = xPos
	tNPC[nIndex].yPos = yPos
	tNPC[nIndex].zPos = zPos
end

function SetPrize( strWeek, strItemId, nItemNum, byFlag, nPenyaRate )
	if( IsLoadable() == false ) then
		return
	end

	if( tPrize[strWeek] == nil ) then tPrize[strWeek] = {} end
	local tTemp = tPrize[strWeek]
	local nIndex = #tTemp + 1
	
	tTemp[nIndex] = {}
	tTemp[nIndex].strItemId = strItemId
	tTemp[nIndex].nItemNum = nItemNum
	tTemp[nIndex].byFlag = byFlag
	
	if( tTime[strWeek] ~= nil ) then 
		tTemp[nIndex].nPenya = tTime[strWeek].nPayPenya * ( nPenyaRate / 100 )
	else
		tTemp[nIndex].nPenya = 1000000 * ( nPenyaRate / 100 )
	end
end

function GetPrize()
	return tPrize[os.date("%a")]
end

function SetMiniGamePrize( strWeek, nIndex, strItemId, nItemNum, byFlag )
	if( IsLoadable() == false ) then
		return
	end
	
	if( tMiniGamePrize[strWeek] == nil ) then
		tMiniGamePrize[strWeek] = {}
	end
	
	local tTemp = tMiniGamePrize[strWeek]

	tTemp[nIndex] = {}
	tTemp[nIndex].strItemId = strItemId
	tTemp[nIndex].nItemNum = nItemNum
	tTemp[nIndex].byFlag = byFlag
end

function GetMiniGamePrize( nCompletedNum )
	local tTemp = tMiniGamePrize[os.date("%a")]
	if( tTemp == nil ) then return nil end
	return tMiniGamePrize[os.date("%a")][nCompletedNum]
end
