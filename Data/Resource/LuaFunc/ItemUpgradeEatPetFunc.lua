--------------------------------------------------------------------
-- °¢¼º ------------------------------------------------------------
--------------------------------------------------------------------

tRandomOptionExtension = {}

function AddRandomOptionExtension( strItemKind1, strItemKind2, strItemKind3 )
	local nIndex = #tRandomOptionExtension + 1

	tRandomOptionExtension[nIndex] = {}
	tRandomOptionExtension[nIndex].strItemKind1 = strItemKind1
	tRandomOptionExtension[nIndex].strItemKind2 = strItemKind2
	tRandomOptionExtension[nIndex].strItemKind3 = strItemKind3
	tRandomOptionExtension[nIndex].dwMaxSize = 0
	tRandomOptionExtension[nIndex].tSizeProb = {}
	tRandomOptionExtension[nIndex].tDstProb = {}
end

function SetMaxSize( dwMaxSize )
	local nIndex 	= #tRandomOptionExtension 
	tRandomOptionExtension[nIndex].dwMaxSize = dwMaxSize
end

function SetSizeProb( dwProb1, dwProb2, dwProb3, dwProb4 )
	local nGIndex 	= #tRandomOptionExtension 
	local nSIndex	= #tRandomOptionExtension[nGIndex].tSizeProb  + 1

	tRandomOptionExtension[nGIndex].tSizeProb[nSIndex] = {}
	tRandomOptionExtension[nGIndex].tSizeProb[nSIndex].dwProb1 = dwProb1
	tRandomOptionExtension[nGIndex].tSizeProb[nSIndex].dwProb2 = dwProb2
	tRandomOptionExtension[nGIndex].tSizeProb[nSIndex].dwProb3 = dwProb3
	tRandomOptionExtension[nGIndex].tSizeProb[nSIndex].dwProb4 = dwProb4
end

function SetDstProb( strDst, dwProb )
	local nGIndex 	= #tRandomOptionExtension
	local nDIndex	= #tRandomOptionExtension[nGIndex].tDstProb + 1

	tRandomOptionExtension[nGIndex].tDstProb[nDIndex] = {}
	tRandomOptionExtension[nGIndex].tDstProb[nDIndex].strDst = strDst
	tRandomOptionExtension[nGIndex].tDstProb[nDIndex].dwProb = dwProb
end


tDstParameter = {}

function AddDstParameter( strDst )
	local nIndex = #tDstParameter +1

	tDstParameter[nIndex] = {}
	tDstParameter[nIndex].strDst = strDst
	tDstParameter[nIndex].tAdjProb = {}
end

function SetAdjValueProb( nAdjValue, dwProb )
	local nDIndex 	= #tDstParameter
	local nAIndex	= #tDstParameter[nDIndex].tAdjProb + 1

	tDstParameter[nDIndex].tAdjProb[nAIndex] = {}
	tDstParameter[nDIndex].tAdjProb[nAIndex].nAdjValue = nAdjValue
	tDstParameter[nDIndex].tAdjProb[nAIndex].dwProb = dwProb
end

