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
	int PacketCounter = 0;
	while( GlobalData.ThreadIsRunning == 1 && Param->DataReader->ConnectSocket != INVALID_SOCKET )
	{
		int ReadCount = Param->DataReader->ReceivePacketNonBlocking( (char*)buff, MaxBuffSize );
		if( ReadCount > 0 && GlobalData.ThreadIsRunning == 1 )
		{
			Param->Buffer->StoreNetworkData( buff, ReadCount );
//			printf( "%d) Received %d data\n", PacketCounter++, ReadCount );
		}
	}
	GlobalData.ThreadsAliveCount--;
	delete arg;
	free( buff );
//	GlobalData.ThreadIsRunning = 2;
}

void StartDataFeederThread( ClientNetwork *DataSender, AudioBufferStore *Buffer )
{
	DataFeederThreadParam *Param = new DataFeederThreadParam;
	Param->Buffer = Buffer;
	Param->DataReader = DataSender;
	_beginthread( SoundReadWriteDataThread, 0, (void*)Param );
}