--------------------------------------------------------------------
-- 초기화 ----------------------------------------------------------
--------------------------------------------------------------------
dofile( ".\\LuaFunc\\InstanceDungeonBase.lua" )
--------------------------------------------------------------------
--[[
AddDungeon( "WI_DUNGEON_SECRET_L" )
--{	
	SetClass( dwClass )
	SetLevel( nMinLevel, nMaxLevel )
	SetCoolTime( dwCoolTime )
	SetMonster( nType, strMonsterId, bRed, x, y, z )
			:
			:
	SetTeleportPos( nType, x, y, z )
			:
			:
--}
--]]

--[[
-- Sample
AddDungeon( "WI_DUNGEON_FL_MAS" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 80, 300 )
	SetCoolTime( MIN(1) )
	SetTeleportPos( ID_NORMAL, 695, 90, 684 ) -- 정의하지 않으면 맵에서 설정한 좌표로 이동한다.
	SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	SetTeleportPos( ID_BOSS, 695, 100, 684 )
	SetMonster( ID_NORMAL, "MI_CAITSITH04_1", false, 695, 90, 684 )
	SetMonster( ID_NORMAL, "MI_CAITSITH04_1", false, 695, 90, 684 )
	SetMonster( ID_NORMAL, "MI_CAITSITH04_1", false, 695, 90, 684 )
	SetMonster( ID_MIDBOSS, "MI_AIBATT1", false, 695, 90, 684 )
	SetMonster( ID_BOSS, "MI_AIBATT1", true, 695, 100, 684 )
--}
--]]


AddDungeon( "WI_INSTANCE_OMINOUS" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 80, 300 )
	SetCoolTime( MIN(120) )
	--SetTeleportPos( ID_NORMAL, 1358, 102, 1315 )
	--SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	--SetTeleportPos( ID_BOSS, 695, 100, 684 )
	SetMonster( ID_NORMAL, "MI_PRICKANT04", true, 1280, 101, 1640 )
	SetMonster( ID_NORMAL, "MI_MAULMOUSE04", true, 1234, 101, 1393 )
	SetMonster( ID_NORMAL, "MI_CRIPESCENTIPEDE04", true, 1089, 101, 1590 )
	SetMonster( ID_MIDBOSS, "MI_LYCANOS01", true, 1078, 101, 1359 )
	SetMonster( ID_BOSS, "MI_VEMPAIN01", true, 1079, 101, 1457 )
--}

AddDungeon( "WI_INSTANCE_OMINOUS_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 1, 300 )
	SetCoolTime( MIN(120) )
	--SetTeleportPos( ID_NORMAL, 1358, 102, 1315 )
	--SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	--SetTeleportPos( ID_BOSS, 695, 100, 684 )
	SetMonster( ID_NORMAL, "MI_PRICKANT04_1", true, 1280, 101, 1640 )
	SetMonster( ID_NORMAL, "MI_MAULMOUSE04_1", true, 1234, 101, 1393 )
	SetMonster( ID_NORMAL, "MI_CRIPESCENTIPEDE04_1", true, 1089, 101, 1590 )
	SetMonster( ID_MIDBOSS, "MI_LYCANOS01_1", true, 1078, 101, 1359 )
	SetMonster( ID_BOSS, "MI_VEMPAIN01_1", true, 1079, 101, 1457 )
--}

AddDungeon( "WI_INSTANCE_DREADFULCAVE" )
--{
	SetClass( CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 121, 300)
	SetCoolTime( MIN(180) )
	--SetTeleportPos( ID_NORMAL, 1358, 102, 1315 )
	--SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	--SetTeleportPos( ID_BOSS, 695, 100, 684 )

	SetMonster( ID_MIDBOSS, "MI_DREADSTONE01", false, 928, 101, 198 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE02", false, 1504, 101, 419 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE03", false, 1397, 101, 893 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE04", false, 764, 101, 867 )
	SetMonster( ID_MIDBOSS, "MI_DREADSTONE05", false, 675, 101, 560 )

	SetMonster( ID_BOSS, "MI_SKELDEVIL", true, 1809, 101, 1395 )
--}

AddDungeon( "WI_INSTANCE_RUSTIA" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 60, 300 )
	SetCoolTime( MIN(60) )
	--SetTeleportPos( ID_NORMAL, 1358, 102, 1315 )
	--SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	--SetTeleportPos( ID_BOSS, 695, 100, 684 )

	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE01", false, 513, 101, 953 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE02", false, 889, 101, 1121 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE03", false, 926, 101, 850 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE04", false, 1247, 101, 1272 )
	

	SetMonster( ID_BOSS, "MI_BESIBIGFOOT01", true, 1126, 103, 1505 )
--}

AddDungeon( "WI_INSTANCE_RUSTIA_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 60, 300 )
	SetCoolTime( MIN(60) )
	--SetTeleportPos( ID_NORMAL, 1358, 102, 1315 )
	--SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	--SetTeleportPos( ID_BOSS, 695, 100, 684 )

	
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE01", false, 513, 101, 953 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE02", false, 889, 101, 1121 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE03", false, 926, 101, 850 )
	SetMonster( ID_MIDBOSS, "MI_RUSTIACRASHGATE04", false, 1247, 101, 1272 )

	SetMonster( ID_BOSS, "MI_BESIBIGFOOT02", true, 1126, 103, 1505 )
--}



AddDungeon( "WI_INSTANCE_BEHAMAH" )
--{
	SetClass( CLASS_LEGEND_HERO )
	SetLevel( 130, 300 )
	SetCoolTime( MIN(360) )
	--SetTeleportPos( ID_NORMAL, 1358, 102, 1315 )
	--SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	--SetTeleportPos( ID_BOSS, 695, 100, 684 )

	
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 697, 100, 375 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 624, 100, 494 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 918, 100, 643 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 898, 100, 785 )
	SetMonster( ID_MIDBOSS, "MI_BEHESTATUE01", false, 580, 100, 748 )

	SetMonster( ID_BOSS, "MI_BEHEMOTH", true, 1261, 101, 1392 )
--}

AddDungeon( "WI_INSTANCE_KALGAS" )
--{
	SetClass( CLASS_LEGEND_HERO )
	SetLevel( 135, 300 )
	SetCoolTime( MIN(360) )
	--SetTeleportPos( ID_NORMAL, 1358, 102, 1315 )
	--SetTeleportPos( ID_MIDBOSS, 695, 90, 684 )
	--SetTeleportPos( ID_BOSS, 695, 100, 684 )

	
	SetMonster( ID_MIDBOSS, "MI_KALGASSTELE01", false, 2807, 303, 801 )
	SetMonster( ID_MIDBOSS, "MI_KALGASSTELE02", false, 1944, 303, 355 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 976, 100, 192 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 980, 100, 209 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 969, 100, 190 )
	SetMonster( ID_MIDBOSS, "MI_KALGASEGG01", false, 974, 100, 213 )

	SetMonster( ID_BOSS, "MI_KALGASBOSS", true, 971, 101, 202 )
--}


AddDungeon( "WI_WORLD_COLOSSEUM", DUNGEON_LEVEL_NORMAL )
--{


	SetClass( CLASS_NORMAL )
	SetLevel( 1, 90 )
	SetCoolTime( MIN(30) )
	SetStartPosition( 1446, 1219 );

	SetPairMonster( 0, "MI_COLOBANG_1", "MI_COLOBANG", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 1, "MI_COLOWAGJAK_1", "MI_COLOWAGJAK", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 2, "MI_COLOREDMANTIS_1", "MI_COLOREDMANTIS", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 3, "MI_COLOJACKTHEHAMMER_1", "MI_COLOJACKTHEHAMMER", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 4, "MI_COLOBERKRO_1", "MI_COLOBERKRO", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 5, "MI_COLOHOWBOW_1", "MI_COLOHOWBOW", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 6, "MI_COLOROKEPELER_1", "MI_COLOROKEPELER", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 7, "MI_COLOSTIMEWORK_1", "MI_COLOSTIMEWORK", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 8, "MI_COLOGRIMONG_1", "MI_COLOGRIMONG", 1000 ,MIN(1), true, 1291, 100, 1231 )
	SetPairMonster( 9, "MI_COLOGRREUNG_1", "MI_COLOGRREUNG", 1000 ,MIN(2), true, 1291, 100, 1231 )
	SetPairMonster( 10, "MI_COLOLUIA_1", "MI_COLOLUIA", 1000 ,MIN(3), true, 1291, 100, 1231 )
	SetPairMonster( 11, "MI_COLOGONGRI_1", "MI_COLOGONGRI", 1000 ,MIN(3), true, 1291, 100, 1231 )
	SetPairMonster( 12, "MI_COLOKEREUN_1", "MI_COLOKEREUN", 1000 ,MIN(10), true, 1291, 100, 1231 )
	
	SetRandomMonster( "MI_GPOTATO02", MIN(30), true, 1291, 100, 1231 )
	


	--있으면 메세지
	SetAutoStart_WaitingTick( MIN(3) )		
	SetStartStage_WaitingTick( SEC(30) )
	SetComplete_WaitingTick( MIN(5) )
	SetRetry_WaitingTick( MIN(5) )
	SetRetryStartStage_WaitingTick( MIN(1) ) 

	
--}



AddDungeon( "WI_WORLD_COLOSSEUM", DUNGEON_LEVEL_HARD )
--{
	SetClass( CLASS_NORMAL,CLASS_MASTER ,CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 1, 150 )	
	SetCoolTime( MIN(60) )
	SetStartPosition( 1446, 1219 );

	
	SetPairMonster( 0, "MI_COLOBIGFOOT_1", "MI_COLOBIGFOOT", 1000 ,MIN(30), true, 1291, 100, 1231 )
	SetPairMonster( 1, "MI_COLOLYCANOS_1", "MI_COLOLYCANOS", 1000 ,MIN(30), true, 1291, 100, 1231 )
	SetPairMonster( 2, "MI_COLOLUCIFER_1", "MI_COLOLUCIFER", 1000 ,MIN(41), true, 1291, 100, 1231 )		
	SetPairMonster( 3, "MI_COLOSOULVAMPAIN_1", "MI_COLOSOULVAMPAIN", 1000 ,MIN(27), true, 1291, 100, 1231 )
	SetPairMonster( 4, "MI_COLOTUTTLEKING_1", "MI_COLOTUTTLEKING", 1000 ,MIN(9), true, 1291, 100, 1231 )
	SetPairMonster( 5, "MI_COLOSKELGENERAL_1", "MI_COLOSKELGENERAL", 1000 ,MIN(38), true, 1291, 100, 1231 )
	SetPairMonster( 6, "MI_COLOSKELDEVIL_1", "MI_COLOSKELDEVIL", 1000 ,MIN(41), true, 1291, 100, 1231 )
	SetPairMonster( 7, "MI_COLORYCANBARGA_1", "MI_COLORYCANBARGA", 1000 ,MIN(120), true, 1291, 100, 1231 )
	SetPairMonster( 8, "MI_COLOANGRYBEHEMOTH_1", "MI_COLOANGRYBEHEMOTH", 1000 ,MIN(120), true, 1291, 100, 1231 )


	SetRandomMonster( "MI_GPOTATO01", MIN(30), true, 1291, 100, 1231 )

	--있으면 메세지
	SetAutoStart_WaitingTick( MIN(3) )		
	SetStartStage_WaitingTick( SEC(30) )
	SetComplete_WaitingTick( MIN(5) )
	SetRetry_WaitingTick( MIN(5) )
	SetRetryStartStage_WaitingTick( MIN(1) ) 

	
--}

AddDungeon( "WI_INSTANCE_UPRESIA" )
--{
	SetClass( CLASS_NORMAL,CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 75 , 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_BOSS, "MI_DREAMQEEN01", true, 2151, 100, 1727 )

--}



AddDungeon( "WI_INSTANCE_UPRESIA_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO)
	SetLevel( 75 , 300 )
	SetCoolTime( MIN(90) )
	SetMonster( ID_BOSS, "MI_DREAMQEEN01_1", true, 2151, 100, 1727 )	
--}



AddDungeon( "WI_INSTANCE_HERNEOS" )
--{
	SetClass( CLASS_NORMAL,CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 90 , 300 )
	SetCoolTime( MIN(90) )

	SetMonster( ID_MIDBOSS, "MI_HERNSIREN01", true, 689, 100, 508 )

	SetMonster( ID_BOSS, "MI_HERNKRAKEN01", true, 1942, 100, 1424 )	
--}



AddDungeon( "WI_INSTANCE_HERNEOS_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 90 , 300 )
	SetCoolTime( MIN(90) )

	SetMonster( ID_MIDBOSS, "MI_HERNSIREN01_1", true, 689, 100, 508 )

	SetMonster( ID_BOSS, "MI_HERNKRAKEN01_1", true, 1942, 100, 1424 )
--}



AddDungeon( "WI_INSTANCE_SANPRES" )
--{
	SetClass( CLASS_NORMAL, CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO )
	SetLevel( 105 , 300 )
	SetCoolTime( MIN(90) )

	SetMonster( ID_MIDBOSS, "MI_SHIPHIPPOGRIPH", true, 1961, 101, 253 )

	SetMonster( ID_BOSS, "MI_SHIPHARPINEES", true, 1362, 109, 1730 )
--}



AddDungeon( "WI_INSTANCE_SANPRES_1" )
--{
	SetClass( CLASS_MASTER, CLASS_HERO, CLASS_LEGEND_HERO)
	SetLevel( 105 , 300 )
	SetCoolTime( MIN(90) )
	
	SetMonster( ID_MIDBOSS, "MI_SHIPHIPPOGRIPH_1", true, 1961, 101, 253 )

	SetMonster( ID_BOSS, "MI_SHIPHARPINEES_1", true, 1362, 109, 1730 )
--}