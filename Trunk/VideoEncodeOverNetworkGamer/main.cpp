#include "StdAfx.h"

GlobalStore GlobalData;

#ifndef ABS
	#define  ABS(a)		((a<0) ? (-a) : (a))
#endif

//#pragma comment(lib, "zlib.lib")

void CaptureScreen()
{
	if( GlobalData.CapturedScreen != NULL )
		delete GlobalData.CapturedScreen;
	GlobalData.CapturedScreen = new CScreenImage;

	if( GlobalData.WndSrc == NULL )
		GlobalData.CapturedScreen->CaptureScreenConvert();
	else
		GlobalData.CapturedScreen->CaptureWindowConvert( GlobalData.WndSrc );
}

void ScreenCaptureAndSendThread( void *arg )
{

	GlobalData.CapturedScreen = NULL;
	z_stream zlib_stream;
	zlib_stream.zalloc = Z_NULL;
	zlib_stream.zfree  = Z_NULL;
	zlib_stream.opaque = Z_NULL;
	unsigned char		*ZlibOutputBuffer = NULL;

	//do we have a source window or a desktop ?
	int SrcWidth, SrcHeight;
	if( GlobalData.WindowName[0] != 0 )
	{
		GlobalData.WndSrc = FindWindowWithNameNonThreaded( GlobalData.WindowName );
		CRect rect;
		::GetWindowRect( GlobalData.WndSrc, rect );
		SrcWidth = rect.right - rect.left;
		SrcHeight = rect.bottom - rect.top;
	}
	else
	{
		GlobalData.WndSrc = NULL;
		GetDesktopResolution( SrcWidth, SrcHeight );
	}

	//check if user messed up aspect ratio
	MEImageDescRGB32 NewSize;
	NewSize.EndX = 0;
	if( GlobalData.ResizeWidth > 0 && GlobalData.ResizeHeight > 0 
//		&& GlobalData.ResizeWidth < SrcWidth && GlobalData.ResizeHeight < SrcHeight		//need to implement upsampling
		)
	{
		float SrcAspectRatio = (float)SrcWidth / (float)SrcHeight;
		float DstAspectRatio = (float)GlobalData.ResizeWidth / (float)GlobalData.ResizeHeight;
		if( ABS( DstAspectRatio - SrcAspectRatio ) > DstAspectRatio / SrcAspectRatio * 5 / 100 )
			printf( "Warning. Aspect ratio changes on resize. From %f to %f\n", SrcAspectRatio, DstAspectRatio );

		CaptureScreen();

		NewSize.StartX = 0;
		NewSize.StartY = 0;
		NewSize.EndX = GlobalData.ResizeWidth;
		NewSize.EndY = GlobalData.ResizeHeight;
		NewSize.PixelByteCount = 3;
		NewSize.Stride = NewSize.EndX * NewSize.PixelByteCount;
		NewSize.Data = (unsigned char*)GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff;
	}

	//setup Zlib
	if( GlobalData.CompressionStrength > Z_NO_COMPRESSION )
	{
		if( deflateInit( &zlib_stream, GlobalData.CompressionStrength ) != Z_OK )
			assert( false );
	}

	//only init network if other inits were fine. This is to avoid blocking ports on some error
	ServerNetwork NetworkListener;

	//this is also used for network buffer
	ZlibOutputBuffer = (unsigned char*)malloc( SrcWidth * SrcHeight * RGB_BYTE_COUNT );
//	assert( SrcWidth * SrcHeight * RGB_BYTE_COUNT < GlobalData.MaxPacketSize );

	//start the neverending loop
	StartTimer();
	unsigned int LoopCounter = 1;
	unsigned int FPSSum = 0;
	unsigned int PacketSizeSum = 0;

	printf("Waiting for client connection\n");
	NetworkListener.acceptNewClient();

	GlobalData.ThreadIsRunning = 1;
	while( GlobalData.ThreadIsRunning == 1 )
	{
		unsigned int Start = GetTimer();
		unsigned int End, ClsEnd, EndCapture,EndResize,EndCompress,EndNetwork,EndLoop;
		End = Start;

		if( GlobalData.ShowStatistics == 2 )
		{
//			system("cls");
		}

		printf("press 'e' to shut down properly\n" );

		if( GlobalData.ShowStatistics == 2 )
		{
			ClsEnd = GetTimer();
			printf( "Statistics : Time required for infoprint : %d. Estimated FPS %d\n", ClsEnd - End, 1000 / ( ClsEnd - End + 1) );
			End = ClsEnd;
		}

		CaptureScreen();

		if( GlobalData.ShowStatistics == 2 )
		{
			EndCapture = GetTimer();
			printf( "Statistics : Time required for capture : %d. Estimated FPS %d\n", EndCapture - End, 1000 / ( EndCapture - End + 1) );
			End = EndCapture;
		}
		if( NewSize.EndX != 0 )
		{
			GlobalData.CapturedScreen->Resample( NewSize );
			if( GlobalData.ShowStatistics == 2 )
			{
				EndResize = GetTimer();
				printf( "Statistics : Time required for resize : %d. Estimated FPS %d\n", EndResize - End, 1000 / ( EndResize - End + 1) );
				End = EndResize;
			}
		}
/*		if( LoopCounter == 0 )
		{
			NewSize.Data = (unsigned char*)GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff;
			SaveImageToFile( &NewSize, NULL, "ResizeTest", true, RGB_CHAR_DATA_TYPE );
			exit( 0 );
		}/**/
		if( GlobalData.CompressionStrength > 0 )
		{
			deflateReset( &zlib_stream );

			// set up zlib_stream pointers
			zlib_stream.next_out  = (Bytef*)ZlibOutputBuffer + sizeof( NetworkPacketHeader );
			zlib_stream.avail_out = SrcWidth * SrcHeight * RGB_BYTE_COUNT - sizeof( NetworkPacketHeader );
			zlib_stream.next_in   = (Bytef*)GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff;
			zlib_stream.avail_in  = GlobalData.CapturedScreen->GetRequiredByteCount();

			//deflate
			int ret = deflate( &zlib_stream, Z_FINISH ); 

			//for some strange reason we did not manage to deflate it : destination buffer too small probably
			if( ret != Z_STREAM_END ) 
				assert( 0 );

			assert( zlib_stream.avail_out != 0 && zlib_stream.avail_in == 0 );

			if( GlobalData.ShowStatistics == 2 )
			{
				EndCompress = GetTimer();
				printf( "Statistics : Time required for compress : %d. Estimated FPS %d\n", EndCompress - End, 1000 / ( EndCompress - End + 1) );
				printf( "Statistics : compress in %d, out %d, pct %.3f\n", GlobalData.CapturedScreen->GetRequiredByteCount(), zlib_stream.total_out, (float) zlib_stream.total_out / (float)GlobalData.CapturedScreen->GetRequiredByteCount() );
				End = EndCompress;
			}
		}

//		NetworkListener.acceptNewClient();

		if( NetworkListener.HasConnections() == 0 )
		{
//			printf( "Network : Waiting for clients to connect\n" );
//			NetworkListener.acceptNewClient();
			break;
		}
		else
		{
			NetworkPacketHeader *ph = (NetworkPacketHeader *)ZlibOutputBuffer;
			ph->CompressionStrength = GlobalData.CompressionStrength;
			ph->Version = 1;
			ph->Width = GlobalData.CapturedScreen->ActiveImageWidth;
			ph->Height = GlobalData.CapturedScreen->ActiveImageHeight;
			ph->PixelByteCount = GlobalData.CapturedScreen->ActiveImagePixelByteCount;
			ph->Stride = GlobalData.CapturedScreen->ActiveImageStride;

			if( GlobalData.CompressionStrength > 0 && zlib_stream.avail_out != 0 && zlib_stream.avail_in == 0 && zlib_stream.total_out < GlobalData.CapturedScreen->GetRequiredByteCount() )
			{
				ph->CompressedSize = zlib_stream.total_out;
				ph->PacketSize = ph->CompressedSize + sizeof( NetworkPacketHeader );
				NetworkListener.sendToAll( (char*)ZlibOutputBuffer, ph->CompressedSize + sizeof( NetworkPacketHeader ) );
			}
			else
			{
				memcpy( ZlibOutputBuffer + sizeof( NetworkPacketHeader ), GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff, GlobalData.CapturedScreen->GetRequiredByteCount() );
				ph->CompressedSize = GlobalData.CapturedScreen->GetRequiredByteCount();
				ph->PacketSize = ph->CompressedSize + sizeof( NetworkPacketHeader );
				NetworkListener.sendToAll( (char*)ZlibOutputBuffer, ph->CompressedSize + sizeof( NetworkPacketHeader ) );
			}
			PacketSizeSum += ph->PacketSize;
			if( GlobalData.ShowStatistics == 2 )
			{
				EndNetwork = GetTimer();
				printf( "Statistics : Time required for network send : %d. Estimated FPS %d\n", EndNetwork - End, 1000 / ( EndNetwork - End + 1) );
				printf( "Statistics : Avg packet size %d\n", PacketSizeSum / LoopCounter );
				End = EndNetwork;
			}
		}

		EndLoop = GetTimer();
		FPSSum += EndLoop - Start;
		if( ( GlobalData.ShowStatistics == 1 && LoopCounter % 5 == 0 ) || GlobalData.ShowStatistics > 1 )
		{
			printf( "Statistics : Time required overall : %d. Avg FPS %d\n\n", EndLoop - Start, 1000 / ( FPSSum / LoopCounter ) );
			End = EndLoop;
		}

		if( 1000 / GlobalData.FPSLimit > (int)( EndLoop - Start ) )
			Sleep( 1000 / GlobalData.FPSLimit - ( EndLoop - Start ) );

		LoopCounter++;
	}

	if( GlobalData.CompressionStrength > Z_NO_COMPRESSION && zlib_stream.state != Z_NULL && deflateEnd(&zlib_stream) != Z_OK)
		assert( false );

	WSACleanup();

	GlobalData.ThreadIsRunning = 0;
}

void main()
{
	//load the settings from the ini file
	LoadSettingsFromFile( );

	_beginthread( ScreenCaptureAndSendThread, 0, (void*)NULL);

	char c;
	do {
		c = _getch();
	}while( c != 'e' && GlobalData.ThreadIsRunning != 0 );

	//wait for thread to shut down
	if( GlobalData.ThreadIsRunning != 0 )
		GlobalData.ThreadIsRunning = 2;

	int AntiDeadlock = 10;
	while( GlobalData.ThreadIsRunning != 0 && AntiDeadlock > 0 )
	{
		Sleep( 1000 );
		AntiDeadlock--;
	}

	printf("Shut down complete\n");
}