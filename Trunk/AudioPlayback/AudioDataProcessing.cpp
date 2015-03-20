#include "StdAfx.h"

AudioBufferStore	AudioStore;

void ReadAndRenderSoundUntilNetwork( void *arg )
{
	ClientNetwork *NetworkReader = new ClientNetwork( GlobalData.AudioNetworkIP, GlobalData.AudioNetworkPort );

	if( NetworkReader->ConnectSocket == INVALID_SOCKET )
	{
		printf("could not connect to server\n");
		return;
	}

	printf("Started network reader\n");
	StartDataFeederThread( NetworkReader, &AudioStore );

	printf("Started playback\n");
	int AntiDeadLock = 10;
	while( AudioStore.GetAvailableFrames() == 0 && AntiDeadLock > 0 )
	{
		Sleep( 100 );
		AntiDeadLock--;
	}
	if( AudioStore.GetAvailableFrames() > 0 )
	{
		printf("Received initial packets. Forcing latency %d \n", GlobalData.ForcedAudioLatency);
		Sleep( GlobalData.ForcedAudioLatency );	//to be able to make it echo echo echo echo
		_beginthread( PlayAudioStream, 0, (void*)NULL );

		printf("press any key to exit\n");
		while( GlobalData.ThreadIsRunning == 1 )
			Sleep( 100 );
	}
	else
	{
		printf("Could not read packets from input. Exiting\n");
	}

	NetworkReader->CloseConnection();
	Sleep( 100 );

	delete NetworkReader;
}