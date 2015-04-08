#include "StdAfx.h"

#ifndef ABS
	#define  ABS(a)		((a<0) ? (-a) : (a))
#endif

#define SHOW_STATS_EVERY_N_FRAMES	32

ServerNetwork	*NetworkListener = NULL;
int				IsListeningToNewConnections = 0;

int InitVideoProcessing()
{
	return 0;
}

int InitNetwork()
{
	if( NetworkListener == NULL )
		NetworkListener = new ServerNetwork( GlobalData.VideoNetworkIP, GlobalData.VideoNetworkPort );
	return 0;
}

int InitDataProcessing()
{
	InitCPUUsageMonitor();

	if( InitVideoProcessing() == 0 )
		return InitNetwork();

	return 1;
}

void ShutDownAllDataProcessing( int ShutdownNetwork )
{
	//wait for worker thread to exit
	if( GlobalData.ThreadIsRunning != 0 )
		GlobalData.ThreadIsRunning = 2;

	if( ShutdownNetwork != 0 && NetworkListener != NULL )
		NetworkListener->CloseConnections();

	int AntiDeadlock = 10;
	while( GlobalData.ThreadIsRunning != 0 && AntiDeadlock > 0 )
	{
		Sleep( 1000 );
		AntiDeadlock--;
	}

	if( GlobalData.CapturedScreen != NULL )
	{
		delete GlobalData.CapturedScreen;
		GlobalData.CapturedScreen = NULL;
	}
	if( GlobalData.WindowName != NULL )
	{
		free( GlobalData.WindowName );
		GlobalData.WindowName = NULL;
	}
	GlobalData.WndSrc = 0;
	if( ShutdownNetwork != 0 && NetworkListener != NULL )
	{
		delete NetworkListener;
		NetworkListener = NULL;
		WSACleanup();
	}
}

void CaptureScreen()
{
	if( GlobalData.CapturedScreen != NULL )
		delete GlobalData.CapturedScreen;
	GlobalData.CapturedScreen = new CScreenImage;
	GlobalData.CapturedScreen->CaptureWindowConvert( GlobalData.WndSrc, GlobalData.StartX, GlobalData.StartY, GlobalData.ForcedWidth, GlobalData.ForcedHeight );
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
		GlobalData.WndSrc = FindWindowWithNameNonThreaded( GlobalData.WindowName );
	if( GlobalData.WndSrc == NULL )
		GlobalData.WndSrc = GetDesktopWindow();

	CRect rect,rect2;
	::GetWindowRect( GlobalData.WndSrc, rect );
	int WindowFullWidth = rect.right - rect.left;
	int WindowFullHeight = rect.bottom - rect.top;
//	int BorderThicknessX = GetSystemMetrics(SM_CXSIZEFRAME);
//	int BorderThicknessY = GetSystemMetrics(SM_CYSIZEFRAME);
//	int CaptionThicknessY = GetSystemMetrics(SM_CYSIZEFRAME);
//	int MenuThicknessY = GetSystemMetrics(SM_CYMENU);
	::GetClientRect( GlobalData.WndSrc, rect2 );
	int WindowFullClientWidth = rect2.right - rect2.left;
	int WindowFullClientHeight = rect2.bottom - rect2.top;

	//remove invalid setting values
	if( GlobalData.StartX < 0 || GlobalData.StartX > WindowFullWidth )
		GlobalData.StartX = 0;
	if( GlobalData.StartY < 0 || GlobalData.StartY > WindowFullHeight )
		GlobalData.StartY = 0;
	if( GlobalData.ForcedWidth < 0 || GlobalData.ForcedWidth > WindowFullWidth )
		GlobalData.ForcedWidth = 0;
	if( GlobalData.ForcedHeight < 0 || GlobalData.ForcedHeight > WindowFullHeight )
		GlobalData.ForcedHeight = 0;

	SrcWidth = WindowFullWidth - GlobalData.StartX;
	SrcHeight = WindowFullHeight - GlobalData.StartY;
	if( GlobalData.IncludeWindowBorder == 0 )
	{
		int CaptionThicknessY = 0;
		int BorderWidth = WindowFullWidth - ( rect2.right - rect2.left );
		int BorderHeaders = WindowFullHeight - ( rect2.bottom - rect2.top ) - BorderWidth;
		GlobalData.StartX += rect2.left + ( BorderWidth + 1 )/ 2;
		GlobalData.StartY += rect2.top + BorderHeaders + ( BorderWidth + 1 ) / 2;
		SrcWidth -= ( BorderWidth + 1 ) / 2;
		SrcHeight -= ( ( BorderWidth + 1 ) / 2 + BorderHeaders );
	}

	if( GlobalData.ForcedWidth > 0 )
		SrcWidth = GlobalData.ForcedWidth;
	if( GlobalData.ForcedHeight > 0 )
		SrcHeight = GlobalData.ForcedHeight;

	if( SrcWidth > WindowFullWidth )
		SrcWidth = WindowFullWidth;
	if( SrcHeight > WindowFullHeight )
		SrcHeight = WindowFullHeight;

	//use one single variable everywhere
	if( SrcWidth != WindowFullWidth )
		GlobalData.ForcedWidth = SrcWidth;
	if( SrcHeight != WindowFullHeight )
		GlobalData.ForcedHeight = SrcHeight;

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
	NetworkListener->NetworkUsageStat.Init( 10000 );

	//this is also used for network buffer
	int ZlibBufferSize = VERY_LARGE_RESOLUTION;
//	if( GlobalData.ResizeWidth * GlobalData.ResizeHeight > SrcWidth * SrcHeight )
//		ZlibBufferSize = GlobalData.ResizeWidth * GlobalData.ResizeHeight;
//	else
//		ZlibBufferSize = SrcWidth * SrcHeight;
	ZlibBufferSize = ZlibBufferSize * RGB_BYTE_COUNT + sizeof( NetworkPacketHeader ) + MEMORY_SAFEGUARD_INTRINSIC_IMPLEMENT;
	ZlibOutputBuffer = (unsigned char*)malloc( ZlibBufferSize );
//	assert( SrcWidth * SrcHeight * RGB_BYTE_COUNT < GlobalData.MaxPacketSize );

	//start the neverending loop
	StartTimer();
	int LoopCounter = 1;
	int FPSSum = 0;
	__int64 PacketSizeSum = 0;
	int StartLoopStamp = GetTickCount();
	signed int SleepAdjustment = 0;

//	printf("Waiting for client connection\n");
//	NetworkListener->acceptNewClient();

	if( GlobalData.ShowStatistics == 0 )
		printf("press 'e' to shut down properly\n" );

	GlobalData.ThreadIsRunning = 1;
	while( GlobalData.ThreadIsRunning == 1 )
	{
		//nothing to do right now
		if( NetworkListener->HasConnections() == 0 && GlobalData.ThreadIsRunning == 1 )
		{
			Sleep( 10 );
			continue;
		}

		unsigned int Start = GetTimer();
		unsigned int End, ClsEnd, EndCapture,EndResize,EndCompress,EndNetwork,EndLoop;
		End = Start;

		if( GlobalData.ShowStatistics == 2 )
		{
//			system("cls");
		}

		if( ( GlobalData.ShowStatistics == 1 && LoopCounter % SHOW_STATS_EVERY_N_FRAMES == 0 ) || GlobalData.ShowStatistics > 1 )
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

		if( GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff == NULL )
		{
			printf( "Error - Image Aquire : Could not capture screen\n" );
			break;
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
			zlib_stream.avail_out = ZlibBufferSize - sizeof( NetworkPacketHeader );
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

//		NetworkListener->acceptNewClient();

		if( NetworkListener->HasConnections() == 0 )
		{
//			printf( "Network : Waiting for clients to connect\n" );
//			NetworkListener->acceptNewClient();
//			break;
//			while( GlobalData.ThreadIsRunning == 1 && NetworkListener->HasConnections() == 0 )
//				Sleep( 100 );
			continue;
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
				NetworkListener->sendToAll( (char*)ZlibOutputBuffer, ph->CompressedSize + sizeof( NetworkPacketHeader ) );
			}
			else
			{
				memcpy( ZlibOutputBuffer + sizeof( NetworkPacketHeader ), GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff, GlobalData.CapturedScreen->GetRequiredByteCount() );
				ph->CompressedSize = GlobalData.CapturedScreen->GetRequiredByteCount();
				ph->PacketSize = ph->CompressedSize + sizeof( NetworkPacketHeader );
				NetworkListener->sendToAll( (char*)ZlibOutputBuffer, ph->CompressedSize + sizeof( NetworkPacketHeader ) );
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
		if( ( GlobalData.ShowStatistics == 1 && LoopCounter % SHOW_STATS_EVERY_N_FRAMES == 0 ) || GlobalData.ShowStatistics > 1 )
		{
			unsigned int CurLoopStamp = GetTickCount() + 1;
			printf( "Statistics : Time required overall : %d. AVG FPS %d. Could achieve FPS %d. CPU Usage %.2f. Network usage %d KByte/s \n\n", EndLoop - Start, 1000 / ( ( CurLoopStamp - StartLoopStamp ) / LoopCounter ), 1000 / ( FPSSum / LoopCounter ), (float)GetCurrentCPUUsage(), NetworkListener->NetworkUsageStat.GetSumInterval() / 10 / 1024 );
			End = EndLoop;
		}

		//for external processes
		if( LoopCounter % SHOW_STATS_EVERY_N_FRAMES == 0 )
		{
			unsigned int CurLoopStamp = GetTickCount();
			float MaybeCPUUsage = (float)GetCurrentCPUUsage();
			if( MaybeCPUUsage > 0 )
				GlobalData.StatLastCPUUsage = MaybeCPUUsage;
			GlobalData.StatAvgNetworkUsage = NetworkListener->NetworkUsageStat.GetSumInterval() / 10;
			GlobalData.StatMaxPossibleFPS = 1000 / ( FPSSum / LoopCounter );
			GlobalData.StatAvgFPS = 1000 / ( ( CurLoopStamp - StartLoopStamp ) / LoopCounter );
		}

		if( 1000 / GlobalData.FPSLimit > (int)( EndLoop - Start ) )
		{
			//in theory you can calculate how much you need to sleep. In practice real sleep should be smaller due to thread switch or other delays
			int ShouldSleepMSTheoretical = 1000 / GlobalData.FPSLimit - ( EndLoop - Start );
			int CurLoopStamp = GetTickCount() + 1;
			int PracticalFPS = 1000 / ( ( CurLoopStamp - StartLoopStamp ) / LoopCounter );
			if( PracticalFPS < GlobalData.FPSLimit )
				SleepAdjustment = SleepAdjustment - 1;
			else if( PracticalFPS > GlobalData.FPSLimit )
				SleepAdjustment = SleepAdjustment + 1;
			if( SleepAdjustment > 0 )
				SleepAdjustment = 0;
			if( -SleepAdjustment > ShouldSleepMSTheoretical )
				SleepAdjustment = -(int)ShouldSleepMSTheoretical;
			if( ShouldSleepMSTheoretical + SleepAdjustment > 0 )
				Sleep( ShouldSleepMSTheoretical + SleepAdjustment );
		}

		LoopCounter++;
	}

	if( GlobalData.CompressionStrength > Z_NO_COMPRESSION && zlib_stream.state != Z_NULL && deflateEnd(&zlib_stream) != Z_OK )
		assert( false );

	free( ZlibOutputBuffer );
	GlobalData.ThreadIsRunning = 0;
	printf("Press 'e' to exit in a clean manner\n");
}

void ListenAndAcceptNewClients( void *arg )
{
	if( NetworkListener == NULL )
		return;
	if( IsListeningToNewConnections != 0 )
		return;
	IsListeningToNewConnections = 1;
	printf("Waiting for client connection\n");
	while( GlobalData.ThreadIsRunning == 1 && NetworkListener != NULL )
	{
		bool ret = NetworkListener->acceptNewClient();
		if( ret == false )
		{
			GlobalData.ThreadIsRunning = 2;
			break;
		}
	}
	IsListeningToNewConnections = 0;
}

void StartDataProcessing()
{
	if( NetworkListener == NULL )
		return;
	
	GlobalData.ThreadIsRunning = 1;

	_beginthread( ListenAndAcceptNewClients, 0, (void*)NULL );

	if( GlobalData.ThreadIsRunning == 1 )
		_beginthread( ScreenCaptureAndSendThread, 0, (void*)NULL );
}