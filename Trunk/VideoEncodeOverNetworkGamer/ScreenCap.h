#ifndef _SCREENCAP_H_
#define _SCREENCAP_H_

//struct MEImageDescRGB32;
struct MEImageDescRGB32
{
	unsigned int StartX,StartY;
	unsigned int EndX,EndY;
	unsigned int PixelByteCount;	//kinda hardcoded to 4
	unsigned int Stride;		// should be 3 * width or more. Given in byte count 
	unsigned char *Data;
	int			ErodeRadius,ErodeLimit;
};

class CScreenImage
#if defined( CAN_USE_ATL_IMG ) && !defined( LIB_BUILD )
	: public CImage
#endif
{
public:
	CScreenImage()
	{
		hDIB = NULL;
		ResampleBuff = ActiveRGB4ByteImageBuff = lpbitmap = NULL;
		ActiveImageWidth = ActiveImageStride = ActiveImageHeight = 0;
		MEDesc = NULL;
	}
	~CScreenImage()
	{
		if( hDIB )
		{
			GlobalFree( hDIB );
			ActiveRGB4ByteImageBuff = lpbitmap = NULL;
			ActiveImageWidth = ActiveImageStride = ActiveImageHeight = 0;
			hDIB = NULL;
		}
		if( ResampleBuff != NULL )
		{
			free( ResampleBuff );
			ResampleBuff = NULL;
			ResampledImageWidth = ResampledImageStride = ResampledImageHeight = ResampledImagePixelByteCount = 0;
		}
		if( MEDesc )
		{
			free( MEDesc );
			MEDesc = NULL;
		}
	}
    bool CaptureRect(const CRect& rect) throw();
    bool CaptureRectConvert(const CRect& rect) throw();
    bool CaptureScreen() throw();
    bool CaptureScreenConvert() throw();
    bool CaptureWindow(HWND hWnd) throw();
    bool CaptureWindowConvert(HWND hWnd, int StartX = 0, int StartY = 0, int ForceWidth = 0, int ForceHeight = 0 ) throw();
	void SetToColor( unsigned char R, unsigned char G, unsigned char B );
#if defined( CAN_USE_ATL_IMG ) && !defined( LIB_BUILD )
	void WriteOurBitmapToDC( int Upscale = 1, int ShiftX = 0, int ShiftY = 0, CImage *DestImg = NULL );
#endif
	unsigned int GetRequiredByteCount() { return ActiveImageHeight * ActiveImageStride; }
//junk
	HANDLE	hDIB;
	unsigned char	*lpbitmap;

	unsigned char	*ActiveRGB4ByteImageBuff;	//in case we resize / process the buffer / change location than update this pointer to point to that location
	int				ActiveImageWidth,ActiveImageStride,ActiveImageHeight,ActiveImagePixelByteCount;

	void			Resample( MEImageDescRGB32 &NewSize );
	unsigned char	*ResampleBuff;
	int				ResampledImageWidth,ResampledImageStride,ResampledImageHeight,ResampledImagePixelByteCount;

	BITMAPINFOHEADER	bi;
	MEImageDescRGB32	*MEDesc;
	DWORD dwBmpSize;
};

#endif