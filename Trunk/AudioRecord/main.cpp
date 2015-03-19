#include "StdAfx.h"
#include <conio.h>

GlobalStore GlobalData;

AudioBufferStore AudioStore;

void main()
{
	memset( &GlobalData, 0, sizeof( GlobalData ) );

	AudioStore.SetCacheDuration( 10 );
//	AudioStore.DebugForceStopRecordSeconds = 20;

	ServerNetwork *NetworkSender = new ServerNetwork( "127.0.0.1", "6969" );

	GlobalData.ThreadIsRunning = 1;

	printf("Waiting for connections\n");
	StartWaitAcceptNewConnectionsThread( NetworkSender );

	printf("Started record\n");
	_beginthread( RecordAudioStream, 0, (void*)NULL );

	printf("Trying to send data over network\n");
	StartDataFeederThread( NetworkSender, &AudioStore );

//	Sleep( 1000 );	//infinite echo incoming if tested on same PC
	printf("Started playback\n");
//	_beginthread( PlayAudioStream, 0, (void*)NULL );

	printf("press any key to exit\n");
	_getch();
	GlobalData.ThreadIsRunning = 2;
	NetworkSender->CloseConnections();
	Sleep( 100 );

	int AntiDeadLock = 10;
	while( GlobalData.ThreadsAliveCount > 0 && AntiDeadLock > 0 )
	{
		Sleep( 1000 );
		AntiDeadLock--;
	}

	printf("Done record / send / playback\n");

	delete NetworkSender;
}