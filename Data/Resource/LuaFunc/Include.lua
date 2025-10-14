function Convert( line )
	if( line:find( "#define" ) ~= nil ) then
		line = line:gsub( "%s*#define%s+", "" )
		line = line:gsub( "%b()", "" )
		line = line:gsub( "%s+", "=" )
		if( line:sub( -1 ) == "=" ) then
			line = line:sub( 1, -2 )
		end
		if( line:find( "=" ) ~= nil ) then
			if( select( 2, line:gsub( "=", "=" ) ) > 1 ) then
				line = line:gsub( "=.=", string.sub( line:match( "=.=" ), 2, -2 ) )
			end
			return line.."\n"
		end
	end
	return ""
end

function include( strFile )	
	local strResult = ""
	local bComment = false		
	local script = LoadStream( strFile )		
		
	local file = io.tmpfile()  -- create an empty temporary file
	file:write( script )	
	--fh:flush()  -- 문제가 생기면 주석을 풀어보도록 
	file:seek( "set" )
		
	for line in file:lines() do				
		line = line:gsub( "//.*", "" )
		line = line:gsub( "/%*.-%*/", "" ) 
		if( bComment == false and line:find( "/%*" ) ~= nil ) then 
			line = line:sub( 1, line:find( "/%*" ) - 1 )
			strResult = strResult..Convert( line )
			bComment = true
		end
		if( bComment == true and line:find( "%*/" ) ~= nil ) then 
			line = line:sub( select( 2, line:find( "%*/" ) ) + 1, -1 )
			bComment = false
		end
		if( bComment == false ) then
			strResult = strResult..Convert( line )
		end
	end
	file:close()	
	loadstring( strResult )()	
end