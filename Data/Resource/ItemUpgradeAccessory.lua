--------------------------------------------------------------------
-- 초기화 ----------------------------------------------------------
--------------------------------------------------------------------
dofile( ".\\LuaFunc\\ItemUpgradeBaseFunc.lua" )
--------------------------------------------------------------------


--------------------------------------------------------------------
-- 일반 제련 -------------------------------------------------------
--------------------------------------------------------------------

AddGeneralEnchant( "IK1_GENERAL", "IK2_JEWELRY", "IK3_NONE" )
--{
	SetGeneralMaterial( "IK1_GENERAL", "IK2_MATERIAL", "IK3_SMELT_ACCESSORY_MATERIAL" )

--					제련값,	성공 확률%,	실패/파괴확률%,	마이너스 1 제련값 확률%( 10000 - 성공 확률 - 실패 확률 ) (%값은 n/10000)
	SetGeneralEnchantProb( 		1, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		2, 	10000,		0	)	-- auto --
	SetGeneralEnchantProb( 		3, 	6300,		3700	)	-- auto --
	SetGeneralEnchantProb( 		4, 	4500,		5500	)	-- auto --
	SetGeneralEnchantProb(	 	5, 	3300,		6700	)	-- auto --
	SetGeneralEnchantProb(	 	6, 	2600,		7400	)	-- auto --
	SetGeneralEnchantProb( 		7, 	2100,		7900	)	-- auto --
	SetGeneralEnchantProb( 		8, 	1700,		8300	)	-- auto --
	SetGeneralEnchantProb( 		9, 	1400,		8600	)	-- auto --
	SetGeneralEnchantProb( 		10, 	1100,		8900	)	-- auto --
	SetGeneralEnchantProb( 		11, 	900,		9100	)	-- auto --
	SetGeneralEnchantProb( 		12, 	800,		9200	)	-- auto --
	SetGeneralEnchantProb( 		13, 	600,		9400	)	-- auto --
	SetGeneralEnchantProb( 		14, 	500,		9500	)	-- auto --
	SetGeneralEnchantProb(	 	15, 	400,		9600	)	-- auto --
	SetGeneralEnchantProb(	 	16, 	300,		9700	)	-- auto --
	SetGeneralEnchantProb( 		17, 	200,		9800	)	-- auto --
	SetGeneralEnchantProb( 		18, 	100,		9900	)	-- auto --
	SetGeneralEnchantProb( 		19, 	70,		9930	)	-- auto --
	SetGeneralEnchantProb( 		20, 	10,		9990	)	-- auto --
--}
