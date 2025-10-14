----------------------------------------------------------
-- 加己力访 ----------------------------------------------
----------------------------------------------------------
tAttribute = {}
function AddAttribute( nNum, nProb, nDamageRate, nDefenseRate, nAddAtkDmgRate )
	tAttribute[nNum] = {}
	tAttribute[nNum].nProb = nProb
	tAttribute[nNum].nDamageRate = nDamageRate
	tAttribute[nNum].nDefenseRate = nDefenseRate
	tAttribute[nNum].nAddAtkDmgRate = nAddAtkDmgRate
end


----------------------------------------------------------
-- 官风唱 力访 -------------------------------------------
----------------------------------------------------------
tBarunaWeaponEnchantProp = {}
function AddBarunaWeaponEnchantProp( nNum, nSuccessProb, nFailProb )
	tBarunaWeaponEnchantProp [nNum] 		= {}
	tBarunaWeaponEnchantProp [nNum].nSuccessProb 	= nSuccessProb
	tBarunaWeaponEnchantProp [nNum].nFailProb 	= nFailProb
end

tBarunaArmorEnchantProp = {}
function AddBarunaArmorEnchantProp( nNum, nSuccessProb, nFailProb )
	tBarunaArmorEnchantProp [nNum] 		= {}
	tBarunaArmorEnchantProp [nNum].nSuccessProb 	= nSuccessProb
	tBarunaArmorEnchantProp [nNum].nFailProb 	= nFailProb
end


