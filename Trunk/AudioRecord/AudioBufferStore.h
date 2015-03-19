#pragma once

//#define SUPPORT_FOR_OS_AUDIO_CONVERSION
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
	#include <mmreg.h> 
	#include <msacm.h>
#endif

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

extern const CLSID CLSID_MMDeviceEnumerator;
extern const IID IID_IMMDeviceEnumerator;
extern const IID IID_IAudioClient;
extern const IID IID_IAudioCaptureClient;
extern const IID IID_IAudioRenderClient;

class AudioBufferStore
{
public:
	AudioBufferStore( );
	~AudioBufferStore();
	void SetCacheDuration( int NewDuration );
	HRESULT SetWriteFormat( WAVEFORMATEX *NewFormat );
	HRESULT SetReadFormat( WAVEFORMATEX *NewFormat );
	HRESULT StoreData( unsigned char *Data, int FrameCount, DWORD Flags, int *done );
    HRESULT LoadData( int BufferFrameCount, BYTE *Data, DWORD *flags );
	int GetAvailableFrames() { return (TotalBytesStored - TotalBytesRead); }
	int	GetRequiredNetworkBufferSize();
	int GetNetworkPacket( unsigned char *buff, int BuffSize );
	void StoreData( unsigned char *Data, int ByteCount );
	void StoreNetworkData( unsigned char *Data, int size );

	unsigned char			*CircularBuffer;
	int						WriteIndex;
	int						ReadIndex;
	int						CacheDuration;
	int						CircularBufferSize;
	long					TotalBytesStored;
	long					TotalBytesRead;
	WAVEFORMATEXTENSIBLE	wfxRead,wfxWrite;
	int						DebugForceStopRecordSeconds;
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
	HACMSTREAM				AudioConverter;
#endif
};
