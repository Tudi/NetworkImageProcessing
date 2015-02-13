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
		GlobalData.CapturedScreen->CaptureWindow( GlobalData.WndSrc );
}

void ScreenCaptureAndSendThread( void *arg )
{

	GlobalData.CapturedScreen = NULL;
	z_stream zlib_stream;
	zlib_stream.zalloc = Z_NULL;
	zlib_stream.zfree  = Z_NULL;
	zlib_stream.opaque = Z_NULL;
	unsigned char		*ZlibOutputBuffer = NULL;

	ServerNetwork NetworkListener;

	//do we have a source window or a desktop ?
	int SrcWidth, SrcHeight;
	if( GlobalData.WindowName[0] != 0 )
	{
		GlobalData.WndSrc = FindWindowWithNameNonThreaded( GlobalData.WindowName );
		CRect rect;
		::GetWindowRect( GlobalData.WndSrc, rect );
		SrcWidth = rect.right;
		SrcHeight = rect.bottom;
	}
	else
	{
		GlobalData.WndSrc = NULL;
		GetDesktopResolution( SrcWidth, SrcHeight );
	}

	//check if user messed up aspect ratio
	MEImageDescRGB32 NewSize;
	if( GlobalData.ResizeWidth > 0 )
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
	//this is also used for network buffer
	ZlibOutputBuffer = (unsigned char*)malloc( SrcWidth * SrcHeight * RGB_BYTE_COUNT );
	assert( SrcWidth * SrcHeight * RGB_BYTE_COUNT < GlobalData.MaxPacketSize );

	//start the neverending loop
	StartTimer();
	unsigned int LoopCounter = 0;
	unsigned int FPSSum = 0;

	GlobalData.ThreadIsRunning = 1;
	system("MODE CON COLS=5 LINES=2");
	while( GlobalData.ThreadIsRunning == 1 )
	{
		unsigned int Start = GetTimer();
		unsigned int End, ClsEnd, EndCapture,EndResize,EndCompress,EndLoop;
		End = Start;

		printf("press 'e' to shut down properly\n" );

		if( GlobalData.ShowStatistics == 2 )
		{
			system("cls");
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
		if( GlobalData.ResizeWidth != -1 )
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
			zlib_stream.avail_out = SrcWidth * SrcHeight * RGB_BYTE_COUNT;
			zlib_stream.next_in   = (Bytef*)GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff;
			zlib_stream.avail_in  = GlobalData.CapturedScreen->GetRequiredByteCount();

			//deflate
			int ret = deflate(&zlib_stream, Z_FINISH); 

			//for some strange reason we did not manage to deflate it : destination buffer too small probably
			if( ret != Z_STREAM_END ) 
				assert( 0 );

			assert( zlib_stream.avail_out != 0 && zlib_stream.avail_in == 0 );

			//check if we got an even larger output then input. If so then we send it uncompressed
//			if( zlib_stream.total_out + 4 > size ) 
//				return false;

			// fill in the full size of the compressed stream
//			*(uint32*)&zlib_update_buffer[0] = size;

			// send it
//			m_session->OutPacket(SMSG_COMPRESSED_UPDATE_OBJECT, (uint16)zlib_stream.total_out + 4, zlib_update_buffer);

			if( GlobalData.ShowStatistics == 2 )
			{
				EndCompress = GetTimer();
				printf( "Statistics : Time required for compress : %d. Estimated FPS %d\n", EndCompress - End, 1000 / ( EndCompress - End + 1) );
				printf( "Statistics : compress in %d, out %d, pct %.3f\n", GlobalData.CapturedScreen->GetRequiredByteCount(), zlib_stream.total_out, (float) zlib_stream.total_out / (float)GlobalData.CapturedScreen->GetRequiredByteCount() );
				End = EndCompress;
			}
		}

		if( NetworkListener.HasConnections() == 0 )
			printf( "Network : Waiting for clients to connect\n" );
		else
		{
			NetworkPacketHeader *ph = (NetworkPacketHeader *)ZlibOutputBuffer;
			ph->CompressionStrength = GlobalData.CompressionStrength;
			ph->Version = 1;
			ph->Width = GlobalData.CapturedScreen->ActiveImageWidth;
			ph->Height = GlobalData.CapturedScreen->ActiveImageHeight;
			ph->PixelByteCount = GlobalData.CapturedScreen->ActiveImagePixelByteCount;
			ph->Stride = GlobalData.CapturedScreen->ActiveImageStride;

			if( zlib_stream.avail_out != 0 && zlib_stream.avail_in == 0 && zlib_stream.avail_out < GlobalData.CapturedScreen->GetRequiredByteCount() )
				NetworkListener.sendToAll( (char*)ZlibOutputBuffer, zlib_stream.avail_out );
			else
			{
				memcpy( ZlibOutputBuffer + sizeof( NetworkPacketHeader ), GlobalData.CapturedScreen->ActiveRGB4ByteImageBuff, GlobalData.CapturedScreen->GetRequiredByteCount() );
				NetworkListener.sendToAll( (char*)ZlibOutputBuffer, GlobalData.CapturedScreen->GetRequiredByteCount() );
			}
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

	if( zlib_stream.state != Z_NULL && deflateEnd(&zlib_stream) != Z_OK)
		assert( false );

	WSACleanup();
}

void main()
{
	//load the settings from the ini file
	LoadSettingsFromFile( );

	_beginthread( ScreenCaptureAndSendThread, 0, (void*)NULL);

	char c;
	do {
		c = _getch();
	}while( c != 'e' );

	printf("Shut down complete\n");
}