#include "StdAfx.h"

#ifndef ABS
	#define  ABS(a)		((a<0) ? (-a) : (a))
#endif

#define SHOW_STATS_EVERY_N_FRAMES	32

ServerNetwork		*AudioNetworkListener = NULL;
AudioBufferStore	AudioStore;
int					AudioIsListeningToNewConnections = 0;

int InitAudioProcessing()
{
	return 0;
}

int AudioInitNetwork()
{
	if( GlobalData.AudioNetworkPort == NULL )
		return 1;
	if( AudioNetworkListener == NULL )
		AudioNetworkListener = new ServerNetwork( GlobalData.AudioNetworkIP, GlobalData.AudioNetworkPort );
	return 0;
}

int AudioInitDataProcessing()
{
	AudioStore.SetCacheDuration( 10 );

	if( InitAudioProcessing() == 0 )
		return AudioInitNetwork();

	return 1;
}

void AudioShutDownAllDataProcessing( int ShutdownNetwork )
{
	//wait for worker thread to exit
	if( GlobalData.ThreadIsRunning != 0 )
		GlobalData.ThreadIsRunning = 2;
	int AntiDeadlock = 10;
	while( GlobalData.ThreadIsRunning != 0 && AntiDeadlock > 0 )
	{
		Sleep( 1000 );
		AntiDeadlock--;
	}
	if( ShutdownNetwork != 0 && AudioNetworkListener != NULL )
	{
		delete AudioNetworkListener;
		AudioNetworkListener = NULL;
		WSACleanup();
	}
}

void AudioListenAndAcceptNewClients( void *arg )
{
	if( AudioNetworkListener == NULL )
		return;
	if( AudioIsListeningToNewConnections != 0 )
		return;
	AudioIsListeningToNewConnections = 1;
	printf("Waiting for client connection\n");
	while( GlobalData.ThreadIsRunning == 1 && AudioNetworkListener != NULL )
	{
		bool ret = AudioNetworkListener->acceptNewClient();
		if( ret == false )
		{
			GlobalData.ThreadIsRunning = 2;
			break;
		}
	}
	AudioIsListeningToNewConnections = 0;
}

void AudioStartDataProcessingWithAutoRestart( void *arg )
{
	if( AudioNetworkListener == NULL || GlobalData.AudioNetworkPort == NULL )
		return;
	
	GlobalData.ThreadIsRunning = 1;

	_beginthread( AudioListenAndAcceptNewClients, 0, (void*)NULL );

	while( GlobalData.ThreadIsRunning == 1 && AudioNetworkListener->HasConnections() == 0 )
		Sleep( 100 );

	if( GlobalData.ThreadIsRunning == 1 && AudioNetworkListener->HasConnections() > 0 )
	{
//		printf("Started record\n");
		_beginthread( RecordAudioStream, 0, (void*)NULL );

//		printf("Trying to send data over network\n");
		StartDataFeederThread( AudioNetworkListener, &AudioStore );
	}
}

void AudioStartDataProcessing()
{
	_beginthread( AudioStartDataProcessingWithAutoRestart, 0, (void*)NULL );
}