#include "StdAfx.h"
#include <stdio.h>

#define FILE_LINE_LENGTH	1024 * 65

void ConvertLineToParam( char *Line )
{
	if( Line[0] == '#' )
	{
//		printf("Read comment line : %s\n", Line );
		return;
	}
	char *ValueStart = strstr( Line, "=" );
	if( ValueStart != NULL && ValueStart != Line )
	{
		ValueStart++;
		char	StrValue[ FILE_LINE_LENGTH ];
		strcpy_s( StrValue, FILE_LINE_LENGTH, ValueStart );

		if( strstr( Line, "ResizeToWidth" ) )
			GlobalData.ResizeWidth = atoi( StrValue );
		else if( strstr( Line, "ResizeToHeight" ) )
			GlobalData.ResizeHeight = atoi( StrValue );
		else if( strstr( Line, "WindowName" ) )
			GlobalData.WindowName = _strdup( StrValue );
		else if( strstr( Line, "ResizeMethod" ) )
			GlobalData.ResizeMethod = atoi( StrValue );
		else if( strstr( Line, "CompresionStrength" ) )
			GlobalData.CompressionStrength = atoi( StrValue );
		else if( strstr( Line, "FPSLimit" ) )
			GlobalData.FPSLimit = atoi( StrValue );
		else if( strstr( Line, "ShowStatistics" ) )
			GlobalData.ShowStatistics = atoi( StrValue );
//		else if( strstr( Line, "EncoderIP" ) )
//			GlobalData.EncoderIP = _strdup( StrValue );
		else if( strstr( Line, "ListenPort" ) )
			GlobalData.EncoderPort = _strdup( StrValue );
//		else if( strstr( Line, "MaxNetworkPacketSize" ) )
//			GlobalData.MaxPacketSize = atoi( StrValue );
	}
}

int ReadLine( FILE *file, char *Store, int MaxLen )
{
	int StoreIndex = 0;
	Store[ 0 ] = 0;

	char ch = getc(file);
	if( ch == EOF )
		return 0;
    while( (ch != '\n') && (ch != '\r') && (ch != EOF) && StoreIndex < MaxLen ) 
	{
        Store[ StoreIndex ] = ch;
        StoreIndex++;
        ch = getc( file );
    }
    Store[ StoreIndex++ ] = '\0';
	return StoreIndex;
}

void LoadSettingsFromFile( )
{
	FILE *inf;
	errno_t err = fopen_s( &inf, "Config.txt", "rt" );
	if( inf )
	{
		char buff[ FILE_LINE_LENGTH ];
		int ReadRet;

		//read while comments until EOF
		while( ReadRet = ReadLine( inf, buff, FILE_LINE_LENGTH ) )
			ConvertLineToParam( buff );

		fclose( inf );
	}
}