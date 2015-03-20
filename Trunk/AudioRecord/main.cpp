#include "StdAfx.h"
#include <conio.h>

GlobalStore GlobalData;

void main()
{
	LoadSettingsFromFile( "../VideoEncodeOverNetworkGamer/Config.txt" );

	AudioInitDataProcessing();

	AudioStartDataProcessing();

//	Sleep( 1000 );	//infinite echo incoming if tested on same PC
//	printf("Started playback\n");
//	_beginthread( PlayAudioStream, 0, (void*)NULL );

	printf("press any key to exit\n");
	_getch();

	AudioShutDownAllDataProcessing( 1 );

	printf("Done record / send / playback\n");
}