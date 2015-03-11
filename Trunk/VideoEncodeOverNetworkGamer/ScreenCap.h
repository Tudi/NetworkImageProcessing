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
	int		ErodeRadius,ErodeLimit;
};

class CScreenImage : public CImage
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
    BOOL CaptureRect(const CRect& rect) throw();
    BOOL CaptureRectConvert(const CRect& rect) throw();
    BOOL CaptureScreen() throw();
    BOOL CaptureScreenConvert() throw();
    BOOL CaptureWindow(HWND hWnd) throw();
    BOOL CaptureWindowConvert(HWND hWnd) throw();
	void SetToColor( unsigned char R, unsigned char G, unsigned char B );
	void WriteOurBitmapToDC( int Upscale = 1, int ShiftX = 0, int ShiftY = 0, CImage *DestImg = NULL );
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