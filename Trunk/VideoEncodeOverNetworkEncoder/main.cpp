#include "StdAfx.h"

//#define USE_SETPIXEL_INSTEAD_BLT 

int IsPerfectFitWindow( int ExpectedWidth, int ExpectedHeight )
{
	//get border size for this window
	CRect rect;
	::GetClientRect( GlobalData.WndSrc, rect );
	if( ( rect.right - rect.left ) == ExpectedWidth && ( rect.bottom - rect.top ) == ExpectedHeight )
		return 1;
	return 0;
}

void PaintToWindow( NetworkPacketHeader *ph, unsigned char *Pixels )
{
	BITMAPINFO bmi;
	memset(&bmi, 0, sizeof(bmi));
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = ph->Width;
	bmi.bmiHeader.biHeight = ph->Height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	HDC hDC = GetDC( GlobalData.WndSrc );
	char *buffer;
	HBITMAP hDib = CreateDIBSection( hDC, &bmi, DIB_RGB_COLORS, (void **)&buffer, 0, 0);
	if (buffer == NULL) 
	{ 
		return;
	}
	HDC hDibDC = CreateCompatibleDC( hDC );
	HGDIOBJ hOldObj = SelectObject( hDibDC, hDib );

	int DstStride = ((bmi.bmiHeader.biWidth * bmi.bmiHeader.biBitCount + 31) / 32) * bmi.bmiHeader.biBitCount / 8;
	int SrcStride = ph->Stride;
	if( DstStride == SrcStride )
	{
		memcpy( buffer, Pixels, SrcStride * ph->Height );
	}
	else if( ph->PixelByteCount == 3 )
	{
		for( int y = 0; y < ph->Height; y++ )
		{
			unsigned char *src = Pixels + SrcStride * y ;
			unsigned int *dst = (unsigned int *)( buffer + DstStride * y );
			for( int x = 0; x < ph->Width; x++ )
			{
#ifdef USE_SETPIXEL_INSTEAD_BLT
				COLORREF crColor = *(unsigned int*)&src[ x * 3 ] & 0x00FFFFFF;
				SetPixel( hDC, x, y, crColor);
#else
				dst[ x ] = *(unsigned int*)&src[ x * 3 ] & 0x00FFFFFF;
#endif
			}
		}
	}
	else if( ph->PixelByteCount == 4 )
	{
		for( int y = 0; y < ph->Height; y++ )
		{
			unsigned char *src = Pixels + SrcStride * y ;
			unsigned int *dst = (unsigned int *)( buffer + DstStride * y );
#ifdef USE_SETPIXEL_INSTEAD_BLT
			for( int x = 0; x < ph->Width; x++ )
			{
				COLORREF crColor = *(unsigned int*)&src[ x * 4 ];
				SetPixel( hDC, x, y, crColor);
			}
#else
			memcpy( dst, src, SrcStride );
#endif
		}
	}
//	SetDIBits( NULL, hDib, 0, bmi.biHeight, Pixels, &bmi, DIB_RGB_COLORS);
#ifndef USE_SETPIXEL_INSTEAD_BLT
	int res;
	if( IsPerfectFitWindow( ph->Width, ph->Height ) )
		res = BitBlt( hDC, 0, 0, ph->Width, ph->Height, hDibDC, 0, 0, SRCCOPY );
	else
	{
		CRect rect;
		::GetClientRect( GlobalData.WndSrc, rect );
		int Width = rect.right - rect.left;
		int Height = rect.bottom - rect.top;
		res = StretchBlt( hDC, 0, 0, Width, Height, hDibDC, 0, 0, ph->Width, ph->Height, SRCCOPY );
	}
#endif

	DeleteObject( hOldObj );
	DeleteDC( hDibDC );
	DeleteObject( hDib );
	ReleaseDC( GlobalData.WndSrc, hDC );
}

void TestPaintToWindow()
{
	NetworkPacketHeader ph;
	unsigned char *Pixels;

	ph.Width = 320;
	ph.Height = 200;
	ph.PixelByteCount = 3;
	ph.Stride = ph.Width;

	Pixels = (unsigned char *)malloc( ph.Width * ph.Height * ph.PixelByteCount + 3 );
	for( int y = 0; y < ph.Height; y++ )
		for( int x = 0; x < ph.Width; x++ )
		{
			*(int*)&Pixels[ ( y * ph.Stride + x ) * 3 ] = RGB( x, x, x );
		}

//	PaintToWindow( &ph, Pixels );
	SetWindowPos( GlobalData.WndSrc, 0, 0, 0, ph.Width, ph.Height, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE );
	PaintToWindow( &ph, Pixels );
	free( Pixels );
}

void ListenAndPaint( void *arg )
{
//	TestPaintToWindow();
//	return;

	FILE *fp;
	errno_t err = freopen_s( &fp, "Debug.txt", "wt" ,stdout );
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
	unsigned int		ZlibBufferSize;

	ClientNetwork NetworkListener( GlobalData.VideoNetworkIP, GlobalData.VideoNetworkPort );

	if( NetworkListener.ConnectSocket == INVALID_SOCKET )
	{
		fclose( fp );
		return;
	}

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
	ZlibBufferSize = MAX_RESOLUTION_SUPPORTED * RGB_BYTE_COUNT + sizeof( NetworkPacketHeader ) + 128;
	ZlibInputBuffer = (unsigned char*)malloc( ZlibBufferSize );
	ZlibOutputBuffer = (unsigned char*)malloc( ZlibBufferSize );
//	assert( SrcWidth * SrcHeight * RGB_BYTE_COUNT < GlobalData.MaxPacketSize );

	//start the neverending loop
	StartTimer();
	unsigned int LoopCounter = 1;
	unsigned int FPSSum = 0;

	GlobalData.ThreadIsRunning = 1;
	GlobalData.UseCustomPicureSize = 0;
	while( GlobalData.ThreadIsRunning == 1 )
	{
		unsigned int Start = GetTimer();
		unsigned int End, EndCapture,EndCompress,EndLoop;
		NetworkPacketHeader *ph;
		End = Start;

		//wait until we have new data on network
		int RecvCount;
#ifndef USE_NETWORK_BUFFERING
		RecvCount = NetworkListener.receivePackets( (char*)ZlibInputBuffer );
#else
		RecvCount = NetworkListener.ReceivePacketNonBlocking( (char*)ZlibInputBuffer, ZlibBufferSize );
#endif

		if( RecvCount < 0 )
			break;

		ph = (NetworkPacketHeader *)ZlibInputBuffer;
		if( RecvCount != ph->PacketSize )
			printf( "%d Did not read all data from network %d received instead %d - %d\n", LoopCounter, RecvCount, ph->PacketSize, ph->CompressedSize + sizeof( NetworkPacketHeader ) );

		if( GlobalData.ShowStatistics == 2 )
		{
			EndCapture = GetTimer();
			printf( "Statistics : Time required for capture recv : %d. Estimated FPS %d\n", EndCapture - End, 1000 / ( EndCapture - End + 1) );
			End = EndCapture;
		}

#ifndef USE_NETWORK_BUFFERING
		int ReplyBuffer[1];
		ReplyBuffer[0] = 1000 / ( FPSSum / LoopCounter + 1);
		NetworkListener.ReplyToSender( (char*)ReplyBuffer, sizeof( ReplyBuffer ) );
#endif

		if( GlobalData.ShowStatistics == 2 )
		{
			EndCapture = GetTimer();
			printf( "Statistics : Time required for capture send: %d. Estimated FPS %d\n", EndCapture - End, 1000 / ( EndCapture - End + 1) );
			End = EndCapture;
		}

		//is it compressed ?
		if( ph->CompressionStrength > Z_NO_COMPRESSION )
		{
			inflateReset( &zlib_stream );
			zlib_stream.next_out  = (Bytef*)ZlibOutputBuffer;
			zlib_stream.avail_out = ZlibBufferSize;
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

		int VideoStreamSizeChangeDetected = ( GlobalData.Width != ph->Width ) || ( GlobalData.Height != ph->Height );
		GlobalData.Width = ph->Width;
		GlobalData.Height = ph->Height;
		if( ( GlobalData.UseCustomPicureSize == 0 || VideoStreamSizeChangeDetected ) && GlobalData.Width != 0 )
		{
			//get border size for this window
			CRect rect,rect2;
			::GetClientRect( GlobalData.WndSrc, rect2 );
			if( rect2.right != GlobalData.Width || rect2.bottom != GlobalData.Height )
			{
				::GetWindowRect( GlobalData.WndSrc, rect );
				int BorderWidth = ( rect.right - rect.left ) - rect2.right;
				int BorderHeight = ( rect.bottom - rect.top ) - rect2.bottom;

				SetWindowPos( GlobalData.WndSrc, 0, 0, 0, ph->Width + BorderWidth, ph->Height + BorderHeight, SWP_NOMOVE|SWP_NOZORDER|SWP_NOACTIVATE );
			}
		}

		//resize window that we will paint to our destination size
		if( LoopCounter == 1 )
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
		}
		else if( ph->Version == 1 && ph->Width < 2000 && ph->Height < 2000 )
		{
			if( ph->CompressionStrength == Z_NO_COMPRESSION )
				PaintToWindow( ph, ZlibInputBuffer + sizeof( NetworkPacketHeader ) );
			else
				PaintToWindow( ph, ZlibOutputBuffer );
		}
		else
			printf( "Something strange with frame %d. Skipped rendering\n", LoopCounter );

		EndLoop = GetTimer();
		FPSSum += EndLoop - Start;
		if( ( GlobalData.ShowStatistics == 1 && LoopCounter % 5 == 0 ) || GlobalData.ShowStatistics > 1 )
		{
			printf( "Statistics : Time required overall : %d. Avg FPS %d\n\n", EndLoop - Start, 1000 / ( FPSSum / LoopCounter ) );
			End = EndLoop;
		}

		//if we are unable to receive all data sent by gamer, network will congest
//		if( 1000 / GlobalData.FPSLimit > (int)( EndLoop - Start ) )
//			Sleep( 1000 / GlobalData.FPSLimit - ( EndLoop - Start ) );

		LoopCounter++;
	}

	if( zlib_stream.state != Z_NULL && inflateEnd(&zlib_stream) != Z_OK )
		assert( false );

	if( fp )
		fclose( fp );
	NetworkListener.CloseConnection();
}

void ListenAndPaintSelfRestart( void *arg )
{
	while( GlobalData.ThreadIsRunning == 1 )
	{
		ListenAndPaint( NULL );
	}
	GlobalData.ThreadIsRunning = 2;

	if( GlobalData.WndSrc != 0 )
		DestroyWindow( GlobalData.WndSrc );
	exit(0);
}

void ListenAndSoundSelfRestart( void *arg )
{
	while( GlobalData.ThreadIsRunning == 1 )
	{
		ReadAndRenderSoundUntilNetwork( NULL );
		Sleep( 100 );
	}
	GlobalData.ThreadIsRunning = 2;
}

void MyMain( HWND MainWindow )
{
	//load the settings from the ini file
	LoadSettingsFromFile( "Config.txt" );
	GlobalData.WndSrc = MainWindow;

	GlobalData.ThreadIsRunning = 1;
	_beginthread( ListenAndPaintSelfRestart, 0, (void*)NULL);
	_beginthread( ListenAndSoundSelfRestart, 0, (void*)NULL);

}