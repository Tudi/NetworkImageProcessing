#include "StdAfx.h"
#include <process.h>

GlobalStore GlobalData;
ServerNetwork *NetworkListener;

void InitDefaultWorkerThreadSettings()
{
	NetworkListener = NULL;
	memset( &GlobalData, 0, sizeof( GlobalData ) );
	_beginthread( ListenToNewConnectionsThread, 0, (void*)NULL);
	_beginthread( ScreenCaptureAndSendThread, 0, (void*)NULL);
}

void InitScreenCaptureAndSendThread()
{
	NetworkListener = new ServerNetwork;
}

void ShutDownNetworkThreads()
{
	if( NetworkListener != NULL )
		delete NetworkListener;
	NetworkListener = NULL;
	WSACleanup();
}

void IdleUntilCanSendData()
{
	while( GlobalData.ThreadIsRunning == WT_WAITING_TO_START || NetworkListener == NULL || NetworkListener->HasConnections() == false )
		Sleep( 100 );
}

void ListenToNewConnectionsThread( void *arg )
{
	IdleUntilCanSendData();
}

void ScreenCaptureAndSendThread( void *arg )
{
	IdleUntilCanSendData();
}
