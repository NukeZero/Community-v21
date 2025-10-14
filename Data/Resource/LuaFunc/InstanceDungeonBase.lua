nMaxInstanceDungeon = 20

tDungeon = {}
--------------------------------------------------------------------
-- 시간 설정 관련 함수 ---------------------------------------------
--------------------------------------------------------------------
function SEC( a ) return a * 1000 end
function MIN( a ) return SEC( a ) * 60 end



DUNGEON_LEVEL_NORMAL 	= 0
DUNGEON_LEVEL_HARD 	= 1
DUNGEON_LEVEL_MAX 	= 2


function AddDungeon( strWorldId, nDungeonLevel )
	local nIndex = #tDungeon + 1
	
	tDungeon[nIndex] 				= {}
	tDungeon[nIndex].nDungeonLevel 		= nDungeonLevel
	tDungeon[nIndex].strWorldId 			= strWorldId
	tDungeon[nIndex].dwClass 			= 65535
	tDungeon[nIndex].dwCoolTime 		= MIN( 30 )
	tDungeon[nIndex].nMinLevel 		= 1
	tDungeon[nIndex].nMaxLevel			= 120
	tDungeon[nIndex].dwAutoStart_WaitingTick	= 0
	tDungeon[nIndex].dwStageTimeLimit_WaitingTick	= 0
	tDungeon[nIndex].dwComplete_WaitingTick	= 0
	tDungeon[nIndex].dwRetry_WaitingTick		= 0
	tDungeon[nIndex].dwRetryStartStage_WaitingTick = 0
	tDungeon[nIndex].tTeleport 			= {}
	tDungeon[nIndex].tMonster 			= {}
	tDungeon[nIndex].tPairMonster 		= {}
	tDungeon[nIndex].tRandomMonster 		= {}
	tDungeon[nIndex].tRewardItem 		= {}
	tDungeon[nIndex].nRewardGold 		= 0
	tDungeon[nIndex].strMailTitleTID		= 0
	tDungeon[nIndex].strMailTextTID 		= 0
	tDungeon[nIndex].nStartPositionX 		= 0
	tDungeon[nIndex].nStartPositionZ		= 0
	tDungeon[nIndex].bPartyWarp			= 0
	tDungeon[nIndex].nTicketIndex			= 0
end


function SetDungeonLevel( nDungeonLevel )
	local nIndex 	= #tDungeon
	local dwClass 	= 0
	if( arg.n > 0 ) then
		for indx = 1, arg.n do
			dwClass = dwClass + arg[indx]
		end
	end
	tDungeon[nIndex].dwClass = dwClass
end


CLASS_NORMAL		= 1
CLASS_MASTER		= 2
CLASS_HERO		= 4
CLASS_LEGEND_HERO	= 8

function SetClass( ... )
	local nIndex = #tDungeon
	local arg = table.pack(...)
	local dwClass = 0
	if( #arg > 0 ) then
		for indx = 1, #arg do
			dwClass = dwClass + arg[indx]
		end
	end
	tDungeon[nIndex].dwClass = dwClass
end


function SetLevel( nMinLevel, nMaxLevel )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].nMinLevel 	= nMinLevel
	tDungeon[nIndex].nMaxLevel 	= nMaxLevel
end


function SetCoolTime( dwCoolTime )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].dwCoolTime = dwCoolTime
end

function SetPartyWarp( bPartyWarp )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].bPartyWarp	= bPartyWarp
end

function SetTicketIndex( strTicketIndex )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].strTicketIndex	= strTicketIndex
end



ID_NORMAL	= 0
ID_MIDBOSS 	= 1
ID_BOSS		= 2


function SetMonster( nStage, strMonsterId, bRed, x, y, z )
	local nPDIndex 	= #tDungeon
	local nMTIndex 	= #tDungeon[nPDIndex].tMonster + 1
		
	tDungeon[nPDIndex].tMonster[nMTIndex] 		= {}
	tDungeon[nPDIndex].tMonster[nMTIndex].nStage 	= nStage
	tDungeon[nPDIndex].tMonster[nMTIndex].strMonsterId 	= strMonsterId
	tDungeon[nPDIndex].tMonster[nMTIndex].bRed 		= bRed
	tDungeon[nPDIndex].tMonster[nMTIndex].x 		= x
	tDungeon[nPDIndex].tMonster[nMTIndex].y 		= y
	tDungeon[nPDIndex].tMonster[nMTIndex].z 		= z
end


function SetTeleportPos( nType, x, y, z )
	local nPDIndex 	= #tDungeon
	local nTPIndex 	= #tDungeon[nPDIndex].tTeleport + 1
		
	tDungeon[nPDIndex].tTeleport[nTPIndex] 	= {}
	tDungeon[nPDIndex].tTeleport[nTPIndex].nType 	= nType
	tDungeon[nPDIndex].tTeleport[nTPIndex].x 	= x
	tDungeon[nPDIndex].tTeleport[nTPIndex].y 	= y
	tDungeon[nPDIndex].tTeleport[nTPIndex].z	= z
end

function SetStartPosition( x, z )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].nStartPositionX  = x
	tDungeon[nIndex].nStartPositionZ  = z
end


-- --------------------------------- monster


function SetPairMonster( nStage, strFakeMonsterId, strRealMonsterId, dw10000RealMonsterRate, dwTimeLimit, bRed, x, y, z )
	local nPDIndex 	= #tDungeon
	local nMTIndex 	= #tDungeon[nPDIndex].tPairMonster + 1

	tDungeon[nPDIndex].tPairMonster[nMTIndex] 			= {}
	tDungeon[nPDIndex].tPairMonster[nMTIndex].nStage 		= nStage
	tDungeon[nPDIndex].tPairMonster[nMTIndex].strFakeMonsterId 	= strFakeMonsterId
	tDungeon[nPDIndex].tPairMonster[nMTIndex].strRealMonsterId 	= strRealMonsterId
	tDungeon[nPDIndex].tPairMonster[nMTIndex].dw10000RealMonsterRate= dw10000RealMonsterRate
	tDungeon[nPDIndex].tPairMonster[nMTIndex].dwTimeLimit		= dwTimeLimit
	tDungeon[nPDIndex].tPairMonster[nMTIndex].bRed 		= bRed
	tDungeon[nPDIndex].tPairMonster[nMTIndex].x 			= x
	tDungeon[nPDIndex].tPairMonster[nMTIndex].y 			= y
	tDungeon[nPDIndex].tPairMonster[nMTIndex].z 			= z
end	

function SetRandomMonster( strMonsterId, dwTimeLimit, bRed, x, y, z )
	local nPDIndex 	= #tDungeon
	local nMTIndex 	= #tDungeon[nPDIndex].tMonster + 1
		
	tDungeon[nPDIndex].tRandomMonster[nMTIndex] 		= {}
	tDungeon[nPDIndex].tRandomMonster[nMTIndex].nStage		= 0
	tDungeon[nPDIndex].tRandomMonster[nMTIndex].strMonsterId 	= strMonsterId
	tDungeon[nPDIndex].tRandomMonster[nMTIndex].dwTimeLimit	= dwTimeLimit
	tDungeon[nPDIndex].tRandomMonster[nMTIndex].bRed 		= bRed
	tDungeon[nPDIndex].tRandomMonster[nMTIndex].x 		= x
	tDungeon[nPDIndex].tRandomMonster[nMTIndex].y 		= y
	tDungeon[nPDIndex].tRandomMonster[nMTIndex].z 		= z
end

-- --------------------------------  reward

function SetMailText( strMailTitleTID, strMailTextTID )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].strMailTitleTID 	= strMailTitleTID
	tDungeon[nIndex].strMailTextTID 	= strMailTextTID
end	

function AddRewardItem( strItemID, wCount )
	local nIndex 	= #tDungeon
	local nRWIndex 	= #tDungeon[nIndex].tRewardItem + 1

	tDungeon[nIndex].tRewardItem[nRWIndex]		= {}
	tDungeon[nIndex].tRewardItem[nRWIndex].strItemID	= strItemID
	tDungeon[nIndex].tRewardItem[nRWIndex].wCount 	= wCount 
end	

function AddRewardGold( nCount )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].nRewardGold	= nCount
end	


-- --------------------------------  time

function SetAutoStart_WaitingTick( dwTick ) 
	local nIndex 	= #tDungeon
	tDungeon[nIndex].dwAutoStart_WaitingTick	= dwTick 
end

function SetStartStage_WaitingTick( dwTick ) 
	local nIndex 	= #tDungeon
	tDungeon[nIndex].dwStartStage_WaitingTick	= dwTick 
end


function SetComplete_WaitingTick( dwTick ) 
	local nIndex 	= #tDungeon
	tDungeon[nIndex].dwComplete_WaitingTick	= dwTick 
end

function SetRetry_WaitingTick( dwTick ) 
	local nIndex 	= #tDungeon
	tDungeon[nIndex].dwRetry_WaitingTick		= dwTick 
end

function SetRetryStartStage_WaitingTick( dwTick )
	local nIndex 	= #tDungeon
	tDungeon[nIndex].dwRetryStartStage_WaitingTick	= dwTick 
end

