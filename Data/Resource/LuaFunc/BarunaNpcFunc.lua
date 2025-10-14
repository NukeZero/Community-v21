
tBarunaNPCLevel = {}
function AddBarunaLevel( dwNum, dwExpRequired, dwAdditionalSuccessProp )
	tBarunaNPCLevel[dwNum] 				= {}
	tBarunaNPCLevel[dwNum].dwExpRequired		= dwExpRequired
	tBarunaNPCLevel[dwNum].dwAdditionalSuccessProp  = dwAdditionalSuccessProp
end


tBarunaNPCExpEnchantGeneral = {}
function AddBarunaNPCExpEnchantGeneral( dwUpgradeLevel, dwSuccessExp, dwFailExp, dwMinusExp )
	local nIndex = #tBarunaNPCExpEnchantGeneral + 1

	tBarunaNPCExpEnchantGeneral[nIndex] = {}
	tBarunaNPCExpEnchantGeneral[nIndex].dwUpgradeLevel = dwUpgradeLevel
	tBarunaNPCExpEnchantGeneral[nIndex].dwSuccessExp = dwSuccessExp
	tBarunaNPCExpEnchantGeneral[nIndex].dwFailExp = dwFailExp
	tBarunaNPCExpEnchantGeneral[nIndex].dwMinusExp = dwMinusExp
end


tBarunaNPCExpEnchantAttribute = {}
function AddBarunaNPCExpEnchantAttribute( dwUpgradeLevel, dwSuccessExp, dwFailExp, dwMinusExp )
	local nIndex = #tBarunaNPCExpEnchantAttribute + 1

	tBarunaNPCExpEnchantAttribute[nIndex] = {}
	tBarunaNPCExpEnchantAttribute[nIndex].dwUpgradeLevel = dwUpgradeLevel
	tBarunaNPCExpEnchantAttribute[nIndex].dwSuccessExp = dwSuccessExp
	tBarunaNPCExpEnchantAttribute[nIndex].dwFailExp = dwFailExp
	tBarunaNPCExpEnchantAttribute[nIndex].dwMinusExp = dwMinusExp
end


tBarunaNPCExpRandomOption = {}
function AddBarunaNPCExpRandomOption( dwUpgradeLevel, dwSuccessExp, dwFailExp, dwMinusExp )
	local nIndex = #tBarunaNPCExpRandomOption + 1

	tBarunaNPCExpRandomOption[nIndex] = {}
	tBarunaNPCExpRandomOption[nIndex].dwUpgradeLevel = dwUpgradeLevel
	tBarunaNPCExpRandomOption[nIndex].dwSuccessExp = dwSuccessExp
	tBarunaNPCExpRandomOption[nIndex].dwFailExp = dwFailExp
	tBarunaNPCExpRandomOption[nIndex].dwMinusExp = dwMinusExp
end

