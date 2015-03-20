#include "StdAfx.h"

struct DataFeederThreadParam
{
	ServerNetwork *DataSender;
	AudioBufferStore *Buffer;
};

void SoundReadWriteDataThread( void *arg )
{
	DataFeederThreadParam *Param = (DataFeederThreadParam *)arg;
	int MaxBuffSize = 10 * 64000 * 5 * 4;	//10 seconds, 5 channel, float format, 64khz buffer. i'm expecting 0.10 seconds to be used
	unsigned char *buff = (unsigned char *)malloc( MaxBuffSize );
	GlobalData.ThreadsAliveCount++;
	while( GlobalData.ThreadIsRunning == 1 )
	{
		if( Param->DataSender->HasConnections() == 0 )
		{
			Sleep( 10 );
			continue;
		}
		int CanSendNow = Param->Buffer->GetNetworkPacket( buff, MaxBuffSize );
		if( CanSendNow == 0 )
		{
			Sleep( 10 );
			continue;
		}
//		printf( "Sending %d data\n", CanSendNow );
		Param->DataSender->sendToAll( (char*)buff, CanSendNow );
	}
	GlobalData.ThreadsAliveCount--;
	delete arg;
	free( buff );
}

void StartDataFeederThread( ServerNetwork *DataSender, AudioBufferStore *Buffer )
{
	DataFeederThreadParam *Param = new DataFeederThreadParam;
	Param->Buffer = Buffer;
	Param->DataSender = DataSender;
	_beginthread( SoundReadWriteDataThread, 0, (void*)Param );
}