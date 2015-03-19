#include "StdAfx.h"
#include <process.h>

GlobalStore GlobalData;
AudioBufferStore AudioStore;

void main()
{
	AudioStore.SetCacheDuration( 10 );
	AudioStore.DebugForceStopRecordSeconds = 20;

	ClientNetwork NetworkReader( "127.0.0.1", "6969" );

	if( NetworkReader.ConnectSocket == 0 )
	{
		printf("could not connect to server\n");
		return;
	}

	printf("Started record\n");
//	RecordAudioStream( );
//	_beginthread( RecordAudioStream, 0, (void*)NULL );
//	Sleep( 1000 );	//infinite echo incoming if tested on same PC
//	printf("Done record\n");
	printf("Started playback\n");
	_beginthread( RecvAudioStream, 0, (void*)NULL );
	_beginthread( PlayAudioStream, 0, (void*)NULL );
	printf("Done playback\n");
}