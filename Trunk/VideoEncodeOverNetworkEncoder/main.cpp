#include "StdAfx.h"

void ListenAndPaint( void *arg )
{
	FILE *fp;
	fp=freopen( "Debug.txt", "w" ,stdout );
	if( fp == NULL ) 
	{
		printf("Cannot open file.\n");
		exit(1);
	}

	z_stream zlib_stream;
	zlib_stream.zalloc = Z_NULL;
	zlib_stream.zfree  = Z_NULL;
	zlib_stream.opaque = Z_NULL;
	unsigned char		*ZlibInputBuffer = NULL;
	unsigned char		*ZlibOutputBuffer = NULL;

	ClientNetwork NetworkListener;

	//do we have a source window or a desktop ?
	int SrcWidth, SrcHeight;
	{
		CRect rect;
		::GetWindowRect( GlobalData.WndSrc, rect );
		SrcWidth = rect.right;
		SrcHeight = rect.bottom;
	}

	//check if user messed up aspect ratio

	//setup Zlib
	if( inflateInit( &zlib_stream ) != Z_OK )
		assert( false );
	//this is also used for network buffer
	ZlibInputBuffer = (unsigned char*)malloc( SrcWidth * SrcHeight * RGB_BYTE_COUNT );
	ZlibOutputBuffer = (unsigned char*)malloc( SrcWidth * SrcHeight * RGB_BYTE_COUNT );
	assert( SrcWidth * SrcHeight * RGB_BYTE_COUNT < GlobalData.MaxPacketSize );

	//start the neverending loop
	StartTimer();
	unsigned int LoopCounter = 0;
	unsigned int FPSSum = 0;

	GlobalData.ThreadIsRunning = 1;
	while( GlobalData.ThreadIsRunning == 1 )
	{
		unsigned int Start = GetTimer();
		unsigned int End, EndCapture,EndCompress,EndLoop;
		NetworkPacketHeader *ph;
		End = Start;

		//wait until we have new data on network
		Sleep( 1000 );
		int RecvCount;
		do {
			RecvCount = NetworkListener.receivePackets( (char*)ZlibInputBuffer );
		}while( RecvCount >= 0 );

		if( RecvCount < 0 )
			break;

		if( GlobalData.ShowStatistics == 2 )
		{
			EndCapture = GetTimer();
			printf( "Statistics : Time required for capture : %d. Estimated FPS %d\n", EndCapture - End, 1000 / ( EndCapture - End + 1) );
			End = EndCapture;
		}

		ph = (NetworkPacketHeader *)ZlibInputBuffer;

		//is it compressed ?
		if( ph->CompressionStrength > Z_NO_COMPRESSION )
		{
			inflateReset( &zlib_stream );
			zlib_stream.next_out  = (Bytef*)ZlibOutputBuffer;
			zlib_stream.avail_out = SrcWidth * SrcHeight * RGB_BYTE_COUNT;
			zlib_stream.next_in   = (Bytef*)ZlibInputBuffer + sizeof( NetworkPacketHeader );
			zlib_stream.avail_in  = ph->CompressedSize;
			int ret = inflate( &zlib_stream, Z_FINISH );
            switch (ret) 
			{
				case Z_NEED_DICT:
					ret = Z_DATA_ERROR;    
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
					break;
            }
		}
		if( GlobalData.ShowStatistics == 2 )
		{
			EndCompress = GetTimer();
			printf( "Statistics : Time required for compress : %d. Estimated FPS %d\n", EndCompress - End, 1000 / ( EndCompress - End + 1) );
			End = EndCompress;
		}

		//resize window that we will paint to our destination size
		if( LoopCounter == 0 )
		{
			printf("Got first network packet\n");
			printf("width %d\n",ph->Width);
			printf("Height %d\n",ph->Height);
			printf("Stride %d\n",ph->Stride);
			printf("PixelByteCount %d\n",ph->PixelByteCount);
			printf("Version %d\n",ph->Version);
			printf("CompressionStrength %d\n",ph->CompressionStrength);
			printf("CompressedSize %d\n",ph->CompressedSize);
			printf("UnCompressedSize %d\n", zlib_stream.total_out );
			SetWindowPos( GlobalData.WndSrc, 0, 0, 0, ph->Width, ph->Height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE );
		}

		EndLoop = GetTimer();
		FPSSum += EndLoop - Start;
		if( ( GlobalData.ShowStatistics == 1 && LoopCounter % 5 == 0 ) || GlobalData.ShowStatistics > 1 )
		{
			printf( "Statistics : Time required overall : %d. Avg FPS %d\n\n", EndLoop - Start, 1000 / ( FPSSum / ( LoopCounter + 1 ) ) );
			End = EndLoop;
		}

		if( 1000 / GlobalData.FPSLimit > (int)( EndLoop - Start ) )
			Sleep( 1000 / GlobalData.FPSLimit - ( EndLoop - Start ) );

		LoopCounter++;
	}

	if( zlib_stream.state != Z_NULL && inflateEnd(&zlib_stream) != Z_OK )
		assert( false );
}

void MyMain()
{
	//load the settings from the ini file
	LoadSettingsFromFile( );

	_beginthread( ListenAndPaint, 0, (void*)NULL);

}