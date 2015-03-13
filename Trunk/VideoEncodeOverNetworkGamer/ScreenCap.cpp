#include "StdAfx.h"

//http://www.codeproject.com/Articles/5051/Various-methods-for-capturing-the-screen

bool CScreenImage::CaptureRect(const CRect& rect)
{
#if defined( CAN_USE_ATL_IMG ) && !defined( LIB_BUILD )
   // detach and destroy the old bitmap if any attached
   CImage::Destroy(); //-> this will error because the bitmap is still attached...
#endif
   
   // create a screen and a memory device context
   HDC hDCScreen = ::CreateDC(("DISPLAY"), NULL, NULL, NULL);
   HDC hDCMem = ::CreateCompatibleDC(hDCScreen);
   // create a compatible bitmap and select it in the memory DC
   HBITMAP hBitmap = 
      ::CreateCompatibleBitmap(hDCScreen, rect.Width(), rect.Height());
   HBITMAP hBmpOld = (HBITMAP)::SelectObject(hDCMem, hBitmap);

   // bit-blit from screen to memory device context
   // note: CAPTUREBLT flag is required to capture layered windows
   DWORD dwRop = SRCCOPY | CAPTUREBLT;
   BOOL bRet = ::BitBlt(hDCMem, 0, 0, rect.Width(), rect.Height(), 
                        hDCScreen, 
                        rect.left, rect.top, dwRop);
   // attach bitmap handle to this object
#if defined( CAN_USE_ATL_IMG ) && !defined( LIB_BUILD )
   Attach(hBitmap);
#endif

   // restore the memory DC and perform cleanup
   ::SelectObject(hDCMem, hBmpOld);
   ::DeleteDC(hDCMem);
   ::DeleteDC(hDCScreen);

   return bRet;
}

bool CScreenImage::CaptureRectConvert(const CRect& rect)
{
    HDC hdcMemDC = NULL;
    HBITMAP hbmScreen = NULL;
    BITMAP bmpScreen;

    HDC hdcScreen = ::CreateDC(("DISPLAY"), NULL, NULL, NULL);
    hdcMemDC = CreateCompatibleDC( hdcScreen ); 
    hbmScreen = CreateCompatibleBitmap( hdcScreen, rect.Width(), rect.Height() );
    SelectObject( hdcMemDC, hbmScreen );
    
	BOOL bRet = ::BitBlt(hdcMemDC, 0, 0, rect.Width(), rect.Height(), hdcScreen, rect.left, rect.top, SRCCOPY | CAPTUREBLT);

    // Get the BITMAP from the HBITMAP
    GetObject( hbmScreen, sizeof(BITMAP), &bmpScreen );
     
    bi.biSize = sizeof(BITMAPINFOHEADER);    
    bi.biWidth = bmpScreen.bmWidth;    
    bi.biHeight = bmpScreen.bmHeight;  
    bi.biPlanes = 1;    
    bi.biBitCount = 32;    
    bi.biCompression = BI_RGB;    
    bi.biSizeImage = 0;  
    bi.biXPelsPerMeter = 0;    
    bi.biYPelsPerMeter = 0;    
    bi.biClrUsed = 0;    
    bi.biClrImportant = 0;

    dwBmpSize = ((bmpScreen.bmWidth * bi.biBitCount + 31) / 32) * 4 * bmpScreen.bmHeight;

    hDIB = GlobalAlloc( GHND, dwBmpSize ); 
    ActiveRGB4ByteImageBuff = lpbitmap = (unsigned char *)GlobalLock( hDIB );    
	ActiveImageWidth = bi.biWidth;
	ActiveImageHeight = bi.biHeight;
	ActiveImageStride = ((bi.biWidth * bi.biBitCount + 31) / 32) * 4;
	ActiveImagePixelByteCount = bmpScreen.bmBitsPixel / 8;

    GetDIBits( hdcScreen, hbmScreen, 0,  (UINT)bmpScreen.bmHeight, lpbitmap, (BITMAPINFO *)&bi, DIB_RGB_COLORS);

    //Unlock and Free the DIB from the heap
    GlobalUnlock( hDIB );    
       
    DeleteObject(hbmScreen);
    DeleteObject(hdcMemDC);
	::ReleaseDC(NULL,hdcScreen);

	MEDesc = (MEImageDescRGB32*)malloc( sizeof( MEImageDescRGB32 ) );
	MEDesc->StartX = 0;
	MEDesc->StartY = 0;
	MEDesc->EndX = ActiveImageWidth;
	MEDesc->EndY = ActiveImageHeight;
	MEDesc->PixelByteCount = ActiveImagePixelByteCount;
	MEDesc->Stride = ActiveImageStride;
	MEDesc->Data = ActiveRGB4ByteImageBuff;

    return 0;
}

bool CScreenImage::CaptureScreen()
{
	CRect rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
	return CaptureRect(rect);
}

bool CScreenImage::CaptureScreenConvert()
{
	CRect rect(0, 0, ::GetSystemMetrics(SM_CXSCREEN), ::GetSystemMetrics(SM_CYSCREEN));
	return CaptureRectConvert(rect);
}

bool CScreenImage::CaptureWindow(HWND hWnd)
{
   BOOL bRet = FALSE;
   if(::IsWindow(hWnd))
   {
      CRect rect;
      ::GetWindowRect(hWnd, rect);
//	  GetClientRect( hWnd, &rect );
      bRet = CaptureRect(rect);
   }
   return bRet;
}

bool CScreenImage::CaptureWindowConvert(HWND hWnd, int StartX, int StartY, int ForceWidth, int ForceHeight)
{
   BOOL bRet = FALSE;
   if(::IsWindow(hWnd))
   {
      CRect rect;
      ::GetWindowRect(hWnd, rect);
	  rect.left += StartX;
	  rect.top += StartY;
	  if( ForceWidth )
		  rect.right = rect.left + ForceWidth;
	  if( ForceHeight )
		  rect.bottom = rect.top + ForceHeight;
      bRet = CaptureRectConvert( rect );
   }
   return bRet;
}

#if defined( CAN_USE_ATL_IMG ) && !defined( LIB_BUILD )
void CScreenImage::SetToColor( unsigned char R, unsigned char G, unsigned char B )
{
	//copy the bitmap to our buffer
/*	BYTE* byteptr = (unsigned char*)lpbitmap;
	assert( byteptr );

	int stride = ( bi.biWidth * ( 32 / 8) + 3) & ~3;
	for( int y = 0; y < bi.biHeight; y += 1 )
	{
		int MirrorY = bi.biHeight - 1 - y;
	    int rowBase = y * stride;
		for( int x = 0; x < bi.biWidth; x += 1 )
		{
			//pointer arithmetics to find (i,j) pixel colors:
			*( byteptr + rowBase + x * 4 + 2 ) = R;
			*( byteptr + rowBase + x * 4 + 1 ) = G;
			*( byteptr + rowBase + x * 4 + 0 ) = B; 
		}
	}/**/
	for( int y = 0; y < bi.biHeight; y += 1 )
		for( int x = 0; x < bi.biWidth; x += 1 )
			SetPixel( x, y, RGB( R, G, B ) );
}
#endif

#if defined( CAN_USE_ATL_IMG ) && !defined( LIB_BUILD )
void CScreenImage::WriteOurBitmapToDC( int Upscale, int ShiftX, int ShiftY, CImage *DestImg )
{
	if( DestImg == NULL ) 
		DestImg = this;
	if( ActiveImagePixelByteCount >= 3 )
	{
		for( int y = 0; y < ActiveImageHeight; y += 1 )
		{
			unsigned char *rowstart = (unsigned char *)ActiveRGB4ByteImageBuff + y * ActiveImageStride;
			for( int x = 0; x < ActiveImageWidth; x += 1 )
			{
				DestImg->SetPixel( ShiftX + x, ShiftY + ActiveImageHeight - 1 - y, RGB( rowstart[2], rowstart[1], rowstart[0] ) );
				rowstart += ActiveImagePixelByteCount;
			}
		}
	}
	else if( ActiveImagePixelByteCount == 1 )
	{
		char *BackupData = (char * )malloc( ActiveImageHeight * ActiveImageStride );
		memcpy( BackupData, ActiveRGB4ByteImageBuff, ActiveImageHeight * ActiveImageStride );
//		char *Src = ActiveRGB4ByteImageBuff;
		char *Src = BackupData;
		for( int y = 0; y < ActiveImageHeight-1; y += 1 )
		{
			unsigned char *rowstart = (unsigned char *)Src + y * ActiveImageStride;
			for( int x = 0; x < ActiveImageWidth; x += 1 )
			{
				for( int ty=0;ty<Upscale;ty++)
					for( int tx=0;tx<Upscale;tx++)
						DestImg->SetPixel( ShiftX + x * Upscale + tx, ShiftY + ( ActiveImageHeight - 1 - y ) * Upscale - ty, RGB( rowstart[x], rowstart[x], rowstart[x] ) );
			}
		}
	}
}
#endif

void CScreenImage::Resample( MEImageDescRGB32 &NewSize )
{
	if( ResampleBuff != NULL && ResampledImageHeight * ResampledImageStride < ( (int)NewSize.EndY - (int)NewSize.StartY ) * (int)NewSize.Stride )
	{
		free( ResampleBuff );
		ResampleBuff = NULL;
	}
	if( ResampleBuff == NULL )
	{
		ResampledImageHeight = NewSize.EndY - NewSize.StartY;
		ResampledImageWidth = NewSize.EndX - NewSize.StartX;
		ResampledImageStride = NewSize.Stride;
		ResampledImagePixelByteCount = NewSize.PixelByteCount;
		ResampleBuff = (unsigned char *)malloc( ( ResampledImageHeight + 1 ) * ( ResampledImageStride + 1 ) * NewSize.PixelByteCount );
	}

	if( ResampledImagePixelByteCount == 4 )
		ResampleRGBLiniar4ByteDownsample( (unsigned char*)ActiveRGB4ByteImageBuff, (unsigned char*)ResampleBuff, ActiveImageWidth, ActiveImageHeight, ResampledImageWidth, ResampledImageHeight, ActiveImageStride, ResampledImageStride );
	else if( ResampledImagePixelByteCount == 3 )
	{
		if( GlobalData.ColorFilterMask != 0 )
			ResampleRGBLiniar4to5BitsDownsample( (unsigned char*)ActiveRGB4ByteImageBuff, (unsigned char*)ResampleBuff, ActiveImageWidth, ActiveImageHeight, ResampledImageWidth, ResampledImageHeight, ActiveImageStride, ResampledImageStride );
		else
			ResampleRGBLiniar4to3ByteDownsample( (unsigned char*)ActiveRGB4ByteImageBuff, (unsigned char*)ResampleBuff, ActiveImageWidth, ActiveImageHeight, ResampledImageWidth, ResampledImageHeight, ActiveImageStride, ResampledImageStride );
	}

	ActiveRGB4ByteImageBuff = ResampleBuff;
	ActiveImageWidth = ResampledImageWidth;
	ActiveImageHeight = ResampledImageHeight;
	ActiveImageStride = ResampledImageStride;
	ActiveImagePixelByteCount = ResampledImagePixelByteCount;
}
