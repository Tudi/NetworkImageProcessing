#include "StdAfx.h"
#include <conio.h>

GlobalStore			GlobalData;

void main()
{
	LoadSettingsFromFile( "../VideoEncodeOverNetworkEncoder/Config.txt" );

	AudioStore.SetCacheDuration( 10 );
//	AudioStore.DebugForceStopRecordSeconds = 20;

	_beginthread( ReadAndRenderSoundUntilNetwork, 0, (void*)NULL );

	printf("press any key to exit\n");
	_getch();
	GlobalData.ThreadIsRunning = 2;

	int AntiDeadLock = 10;
	while( GlobalData.ThreadsAliveCount > 0 && AntiDeadLock > 0 )
	{
		Sleep( 1000 );
		AntiDeadLock--;
	}

	printf("Done recv / playback\n");
}