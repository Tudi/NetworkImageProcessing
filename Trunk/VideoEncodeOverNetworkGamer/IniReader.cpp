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
		else if( strstr( Line, "IncludeBorder" ) )
			GlobalData.IncludeWindowBorder = atoi( StrValue );
		else if( strstr( Line, "CaptureStartX" ) )
			GlobalData.StartX = atoi( StrValue );
		else if( strstr( Line, "CaptureStartY" ) )
			GlobalData.StartY = atoi( StrValue );
		else if( strstr( Line, "CaptureWidth" ) )
			GlobalData.ForcedWidth = atoi( StrValue );
		else if( strstr( Line, "CaptureHeight" ) )
			GlobalData.ForcedHeight = atoi( StrValue );
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
		else if( strstr( Line, "VideoListenPort" ) )
			GlobalData.VideoNetworkPort = _strdup( StrValue );
		else if( strstr( Line, "AudioListenPort" ) )
			GlobalData.AudioNetworkPort = _strdup( StrValue );
//		else if( strstr( Line, "MaxNetworkPacketSize" ) )
//			GlobalData.MaxPacketSize = atoi( StrValue );
		else if( strstr( Line, "ReduceColorDepth" ) )
		{
			GlobalData.ColorFilterMask = atoi( StrValue );
			if( GlobalData.ColorFilterMask == 2 ) 
				GlobalData.ColorFilterMask = 0x00FEFEFE;
			else if( GlobalData.ColorFilterMask == 3 ) 
				GlobalData.ColorFilterMask = 0x00FCFCFC;
			else if( GlobalData.ColorFilterMask == 4 ) 
				GlobalData.ColorFilterMask = 0x00F8F8F8;
			else if( GlobalData.ColorFilterMask == 5 ) 
				GlobalData.ColorFilterMask = 0x00F0F0F0;
			else
				GlobalData.ColorFilterMask = 0;
		}
		else if( strstr( Line, "VideoCapturePCIP" ) )
			GlobalData.VideoNetworkIP = _strdup( StrValue );
		else if( strstr( Line, "VideoCapturePCPort" ) )
			GlobalData.VideoNetworkPort = _strdup( StrValue );
		else if( strstr( Line, "AudioCapturePCIP" ) )
			GlobalData.AudioNetworkIP = _strdup( StrValue );
		else if( strstr( Line, "AudioCapturePCPort" ) )
			GlobalData.AudioNetworkPort = _strdup( StrValue );
//		else if( strstr( Line, "FPSLimit" ) )
//			GlobalData.FPSLimit = atoi( StrValue );
//		else if( strstr( Line, "MaxNetworkPacketSize" ) )
//			GlobalData.MaxPacketSize = atoi( StrValue );
		else if( strstr( Line, "ShowStatistics" ) )
			GlobalData.ShowStatistics = atoi( StrValue );
		else if( strstr( Line, "ForcedAudioLatency" ) )
			GlobalData.ForcedAudioLatency = atoi( StrValue );
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

void LoadSettingsFromFile( char *FileName )
{
	memset( &GlobalData, 0, sizeof( GlobalData ) );

	FILE *inf;
	if( FileName != NULL )
		errno_t err = fopen_s( &inf, FileName, "rt" );
	else
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