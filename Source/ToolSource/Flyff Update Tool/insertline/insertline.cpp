// insertline.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <vector>
using namespace std;

int main(int argc, char* argv[])
{
	if( argc < 3 )
	{
		puts( "please input file name" );
		return 0;
	}
	
	FILE *fp = fopen( argv[1], "r+" );
	if( fp == NULL )
	{
		printf( "file open error %s\n", argv[1] );
		return 0;
	}

	vector< string > lines;
	char buffer[256];
	for( ;; )
	{
		if( fgets( buffer, sizeof(buffer) , fp ) == NULL)
			break;
		lines.push_back( buffer );
	}

	fseek( fp, 0, SEEK_SET );

	for( int i=0; i<lines.size(); ++i )
	{
		if( i == 2 )
			fprintf( fp, "#define %s\n", argv[2] );		
		fprintf( fp, "%s", lines[i].c_str() );
	}

	fclose( fp );
	return 0;
}

