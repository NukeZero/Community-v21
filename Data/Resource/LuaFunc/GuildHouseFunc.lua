function MIN( a ) return a * 1000 * 60 end
--------------------------------------------------------------------
GH_AREA_FLARIS = 1
GH_AREA_SAINTMORNING = 2
GH_AREA_ELIUN = 3
--------------------------------------------------------------------

tTender = {}
tNpc = {}
tGHInfo = {}
tGHQuest = {}
tNoticeTime = {}
tTenderTime = {}



function GetTimeToNumber( strTime )
	local strTemp = ""
	local j = 0
	for i in string.gmatch( strTime, "%d+" ) do
		j = j + 1
		if( (j~=1) and (tonumber(i)<10) ) then
			i = "0"..tonumber(i)
		end
		strTemp = strTemp..i
	end
	return tonumber( strTemp )
end

function SetNoticeTime( nBeginDay, strBeginTime, nEndDay, strEndTime )
	tNoticeTime.nBeginDay = nBeginDay
	tNoticeTime.strBeginTime = strBeginTime
	tNoticeTime.nEndDay = nEndDay
	tNoticeTime.strEndTime = strEndTime
end

function SetTenderTime( nBeginDay, strBeginTime, nEndDay, strEndTime )
	tTenderTime.nBeginDay = nBeginDay
	tTenderTime.strBeginTime = strBeginTime
	tTenderTime.nEndDay = nEndDay
	tTenderTime.strEndTime = strEndTime
end

function IsNoticeTime()
	local nCurrentDay = tonumber( os.date("%w") )
	local nCurrentTime = tonumber( os.date("%H%M") )

	local nBeginTime = GetTimeToNumber( tNoticeTime.strBeginTime )
	local nEndTime = GetTimeToNumber( tNoticeTime.strEndTime )

	if( nCurrentDay == tNoticeTime.nBeginDay ) then
		if( nCurrentTime >= nBeginTime ) then
			return true
		end
	end

	if( nCurrentDay == tNoticeTime.nEndDay ) then
		if( nCurrentTime <= nEndTime ) then
			return true
		end
	end

	if( tNoticeTime.nEndDay < tNoticeTime.nBeginDay ) then
		if( (nCurrentDay > tNoticeTime.nBeginDay) and (nCurrentDay < tNoticeTime.nEndDay + 7) ) then
			return true
		end
	end

	if( tNoticeTime.nEndDay > tNoticeTime.nBeginDay ) then
		if( (nCurrentDay > tNoticeTime.nBeginDay) and (nCurrentDay < tNoticeTime.nEndDay) ) then
			return true
		end
	end

	return false
end

function IsTenderTime()
	local nCurrentDay = tonumber( os.date("%w") )
	local nCurrentTime = tonumber( os.date("%H%M") )

	local nBeginTime = GetTimeToNumber( tTenderTime.strBeginTime )
	local nEndTime = GetTimeToNumber( tTenderTime.strEndTime )

	if( nCurrentDay == tTenderTime.nBeginDay ) then
		if( nCurrentTime >= nBeginTime ) then
			return true
		end
	end

	if( nCurrentDay == tTenderTime.nEndDay ) then
		if( nCurrentTime <= nEndTime ) then
			return true
		end
	end

	if( tTenderTime.nEndDay < tTenderTime.nBeginDay ) then
		if( (nCurrentDay > tTenderTime.nBeginDay) and (nCurrentDay < tTenderTime.nEndDay + 7) ) then
			return true
		end
	end

	if( tTenderTime.nEndDay > tTenderTime.nBeginDay ) then
		if( (nCurrentDay > tTenderTime.nBeginDay) and (nCurrentDay < tTenderTime.nEndDay) ) then
			return true
		end
	end

	return false
end

function SetTenderInfo( strGHType )
	local nIndex = #tTender + 1
	
	tTender[nIndex] = {}
	tTender[nIndex].strGHType = strGHType
	tTender[nIndex].nMaxHouse = 1
	tTender[nIndex].nMinPenya = 1
	tTender[nIndex].nChargeRate = 1
	tTender[nIndex].nPrevNoticeTick = 0
	tTender[nIndex].nTenderNoticeTick = 0
	tTender[nIndex].tNpc = {}
end

function SetMaxTender( nMaxHouse )
	local nIndex = #tTender
	tTender[nIndex].nMaxHouse = nMaxHouse
end

function SetMinTenderPenya( nMinPenya )
	local nIndex = #tTender
	tTender[nIndex].nMinPenya = nMinPenya
end

function SetChargeRate( nChargeRate )
	local nIndex = #tTender
	tTender[nIndex].nChargeRate = nChargeRate
end

function SetNoticeTick( nPrevNoticeTick, nTenderNoticeTick )
	local nIndex =  #tTender 
	tTender[nIndex].nPrevNoticeTick = nPrevNoticeTick
	tTender[nIndex].nTenderNoticeTick = nTenderNoticeTick
end

function SetNpc( strNpcId, nAreaIndex )
	local nIndex = #tTender
	local nNIndex = #tTender[nIndex].tNpc + 1

	tTender[nIndex].tNpc[nNIndex] = {}
	tTender[nIndex].tNpc[nNIndex].strNpcId = strNpcId
	tTender[nIndex].tNpc[nNIndex].nAreaIndex = nAreaIndex
end

function SetGuildHouseInfo( strGHId, strNpcId, strGHType, nAreaIndex, strTitle )
	local nIndex = #tGHInfo + 1

	tGHInfo[nIndex] = {}
	tGHInfo[nIndex].strGHId = strGHId
	tGHInfo[nIndex].strNpcId = strNpcId
	tGHInfo[nIndex].strGHType = strGHType
	tGHInfo[nIndex].nAreaIndex = nAreaIndex
	tGHInfo[nIndex].strTitle = strTitle
end

function SetGuildHouseQuest( strQuestId, strGHType, nLevel, strDoorId )
	local nIndex = #tGHQuest + 1

	tGHQuest[nIndex] = {}
	tGHQuest[nIndex].strQuestId = strQuestId
	tGHQuest[nIndex].strGHType = strGHType
	tGHQuest[nIndex].nLevel = nLevel
	tGHQuest[nIndex].strDoorId = strDoorId
end
