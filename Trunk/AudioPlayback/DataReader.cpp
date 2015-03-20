#include "StdAfx.h"

struct DataFeederThreadParam
{
	ClientNetwork		*DataReader;
	AudioBufferStore	*Buffer;
};

void SoundReadWriteDataThread( void *arg )
{
	DataFeederThreadParam *Param = (DataFeederThreadParam *)arg;
	int MaxBuffSize = 10 * 64000 * 5 * 4;	//10 seconds, 5 channel, float format, 64khz buffer. i'm expecting 0.10 seconds to be used
	unsigned char *buff = (unsigned char *)malloc( MaxBuffSize );
	GlobalData.ThreadsAliveCount++;
	while( GlobalData.ThreadIsRunning == 1 )
	{
		int ReadCount = Param->DataReader->ReceivePacketNonBlocking( (char*)buff, MaxBuffSize );
		Param->Buffer->StoreNetworkData( buff, ReadCount );
//		printf( "Received %d data\n", ReadCount );
	}
	GlobalData.ThreadsAliveCount--;
	delete arg;
	free( buff );
}

void StartDataFeederThread( ClientNetwork *DataSender, AudioBufferStore *Buffer )
{
	DataFeederThreadParam *Param = new DataFeederThreadParam;
	Param->Buffer = Buffer;
	Param->DataReader = DataSender;
	_beginthread( SoundReadWriteDataThread, 0, (void*)Param );
}