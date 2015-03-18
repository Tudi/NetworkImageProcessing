//-----------------------------------------------------------
// Record an audio stream from the default audio capture
// device. The RecordAudioStream function allocates a shared
// buffer big enough to hold one second of PCM audio data.
// The function uses this buffer to stream data from the
// capture device. The main loop runs every 1/2 second.
//-----------------------------------------------------------

#include <windows.h>
#include <mmsystem.h>
#include <audioclient.h>
#include <stdio.h>
#include <avrt.h>
#include <mmdeviceapi.h>
#include <list>
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
	#include <mmreg.h> 
	#include <msacm.h>
#endif

// REFERENCE_TIME time units per second and per millisecond
//#define REFTIMES_PER_SEC  10000000
//#define REFTIMES_PER_MILLISEC  10000
#define AUDIO_SAMPLE_DURATION 100*10000	

#define EXIT_ON_ERROR(hres)  \
              if (FAILED(hres)) { goto Exit; }
#define SAFE_RELEASE(punk)  \
              if ((punk) != NULL)  \
                { (punk)->Release(); (punk) = NULL; }

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

struct AudioBufferHeader
{
	int FrameCount;
	int	Flags;
};

class AudioBufferStore
{
public:
	AudioBufferStore( int CacheDurationSeconds )
	{
		WriteIndex = 0;
		ReadIndex = 0;
		CircularBuffer = NULL;
		CacheDuration = CacheDurationSeconds;
		TotalSamplesStored = 0;
		TotalSamplesRead = 0;
		CircularBufferSize = 0;
		DebugForceStopRecordSeconds = 0;
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
		AudioConverter = NULL;
#endif
	}
	~AudioBufferStore()
	{
		if( CircularBuffer != NULL )
		{
			free( CircularBuffer );
			CircularBuffer = NULL;
		}
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
		if( AudioConverter != NULL )
		{
			MMRESULT mmr = acmStreamClose( AudioConverter, 0 );
			AudioConverter = NULL;
		}
#endif
	}
	HRESULT SetWriteFormat( WAVEFORMATEX *NewFormat )
	{
		memcpy( &wfxWrite, NewFormat, sizeof( wfxWrite ) );
		CircularBufferSize = wfxWrite.nAvgBytesPerSec * CacheDuration;
		CircularBuffer = (unsigned char *)realloc( CircularBuffer, CircularBufferSize + wfxWrite.nAvgBytesPerSec * 4 );
		return S_OK;
	}
	HRESULT SetReadFormat( WAVEFORMATEX *NewFormat )
	{
		memcpy( &wfxRead, NewFormat, sizeof( wfxRead ) );
//		CircularBufferSize = wfxRead.nAvgBytesPerSec * CacheDuration;
//		CircularBuffer = (unsigned char *)realloc( CircularBuffer, CircularBufferSize + wfxRead.nAvgBytesPerSec * 4 );
		return S_OK;
	}
	HRESULT CopyData( unsigned char *Data, int FrameCount, DWORD Flags, int *done )
	{
		int WriteBlockSize = wfxWrite.nChannels * wfxWrite.wBitsPerSample / 8;
		int	TotalBufferSize = FrameCount * WriteBlockSize;

		if( WriteIndex + TotalBufferSize <= CircularBufferSize )
		{
			memcpy( &CircularBuffer[WriteIndex], Data, TotalBufferSize );
			WriteIndex += TotalBufferSize;
		}
		else
		{
			int CanCopyToEnd = CircularBufferSize - WriteIndex;
			if( CanCopyToEnd > 0 )
				memcpy( &CircularBuffer[WriteIndex], &Data[0], CanCopyToEnd );
			int CanCopyBeggining = TotalBufferSize - CanCopyToEnd;
			if( CanCopyBeggining > 0 )
				memcpy( &CircularBuffer[0], &Data[CanCopyToEnd], CanCopyBeggining );
			WriteIndex = CanCopyBeggining;
		}

		TotalSamplesStored += TotalBufferSize;

		//just testing. Remove me later
		if( DebugForceStopRecordSeconds != 0 && TotalSamplesStored > DebugForceStopRecordSeconds * wfxWrite.nAvgBytesPerSec )
			return E_FAIL;

		return S_OK;
	}
    HRESULT LoadData( int BufferFrameCount, BYTE *Data, DWORD *flags )
	{
		*flags = 0;	// need to implement

		//it happens if "sleep" is too small
		if( BufferFrameCount == 0 )
			return S_OK;

		int ReadBlockSize = wfxRead.nChannels * wfxRead.wBitsPerSample / 8;
		int WriteBlockSize = wfxWrite.nChannels * wfxWrite.wBitsPerSample / 8;
		// need to convert sampling rate
		float SamplingRateRatio = ( float )wfxWrite.nSamplesPerSec / ( float ) wfxRead.nSamplesPerSec;
		int	TotalBufferSize = ( (int)( SamplingRateRatio * BufferFrameCount * WriteBlockSize ) * 4 + 3 ) / 4;

		if( TotalSamplesRead + TotalBufferSize > TotalSamplesStored )
			return E_FAIL;

		int IsBothFloatFormat = 0;
		int IsBothPCMFormat = 0;
		if( wfxWrite.wFormatTag == WAVE_FORMAT_EXTENSIBLE && wfxRead.wFormatTag == WAVE_FORMAT_EXTENSIBLE )
		{
			WAVEFORMATEXTENSIBLE *AlternateFormatRead = (WAVEFORMATEXTENSIBLE *)&wfxRead;
			WAVEFORMATEXTENSIBLE *AlternateFormatWrite = (WAVEFORMATEXTENSIBLE *)&wfxWrite;
			IsBothFloatFormat = ( AlternateFormatRead->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT ) && ( AlternateFormatWrite->SubFormat == KSDATAFORMAT_SUBTYPE_IEEE_FLOAT );
			IsBothPCMFormat = ( AlternateFormatRead->SubFormat == KSDATAFORMAT_SUBTYPE_PCM ) && ( AlternateFormatWrite->SubFormat == KSDATAFORMAT_SUBTYPE_PCM );
		}
		else if( wfxWrite.wFormatTag == WAVE_FORMAT_IEEE_FLOAT && wfxRead.wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
			IsBothFloatFormat = 1;
		else if( wfxWrite.wFormatTag == WAVE_FORMAT_PCM && wfxRead.wFormatTag == WAVE_FORMAT_PCM )
			IsBothPCMFormat = 1;
		//pray to the lord jezuz that we guess it. Else expect some noise :(
		if( IsBothPCMFormat == 0 && IsBothFloatFormat == 0 )
			IsBothFloatFormat = 1;

		if( wfxRead.nChannels == wfxWrite.nChannels 
			&& wfxRead.wBitsPerSample == wfxWrite.wBitsPerSample 
			&& wfxRead.nSamplesPerSec == wfxWrite.nSamplesPerSec 
			&& wfxRead.wFormatTag == wfxWrite.wFormatTag )
		{
			if( ReadIndex + TotalBufferSize <= CircularBufferSize )
			{
				memcpy( &Data[0], &CircularBuffer[ReadIndex], TotalBufferSize );
				ReadIndex += TotalBufferSize;
			}
			else
			{
				int CanCopyToEnd = CircularBufferSize - ReadIndex;
				if( CanCopyToEnd > 0 )
					memcpy( &Data[0], &CircularBuffer[ReadIndex], CanCopyToEnd );
				int CanCopyBeggining = TotalBufferSize - CanCopyToEnd;
				if( CanCopyBeggining > 0 )
					memcpy( &Data[CanCopyToEnd], &CircularBuffer[0], CanCopyBeggining );
				ReadIndex = CanCopyBeggining;
			} 
		}
		else if( IsBothFloatFormat == 1 )
		{
			// need to convert channel count
			int WriteBytesPerChannel = wfxWrite.wBitsPerSample / 8;
			int ReadBytesPerChannel = wfxRead.wBitsPerSample / 8;
			// need to convert bits per sample
			for( int i = 0; i < BufferFrameCount; i++ )
			{
				int SourceIndexStart = ((int)(i * SamplingRateRatio) * WriteBlockSize) % CircularBufferSize;
				int SourceIndexEnd = ((int)((i + 1 ) * SamplingRateRatio) * WriteBlockSize) % CircularBufferSize;
				//let's make it mono
				float SampleSum = 0;
				int SampleCount = 0;
				for( int SourceIndex = SourceIndexStart; SourceIndex % CircularBufferSize <= SourceIndexEnd; SourceIndex += WriteBlockSize )
				{
					unsigned char *SourceData = &CircularBuffer[ ( ( ReadIndex + SourceIndex ) % CircularBufferSize ) ];
					for( int SourceChannels = 0; SourceChannels < wfxWrite.nChannels; SourceChannels++ )
					{
						SampleSum += ( *(float*)&SourceData[ SourceChannels * WriteBytesPerChannel ] );
						SampleCount++;
					}
				}
				SampleSum = SampleSum / SampleCount;

				unsigned char *DestData = &Data[ i * ReadBlockSize ];
				for( int DestChannels = 0; DestChannels < wfxRead.nChannels; DestChannels++ )
					*(float*)&DestData[ DestChannels * ReadBytesPerChannel ] = SampleSum;	//wow, will this produce memory write error on last value ?
			}
		}
		else if( IsBothPCMFormat == 1 && wfxRead.wBitsPerSample == wfxWrite.wBitsPerSample && wfxRead.wBitsPerSample == 32 )
		{
			// need to convert channel count
			int WriteBytesPerChannel = wfxWrite.wBitsPerSample / 8;
			int ReadBytesPerChannel = wfxRead.wBitsPerSample / 8;
			// need to convert bits per sample
			for( int i = 0; i < BufferFrameCount; i++ )
			{
				int SourceIndexStart = ((int)(i * SamplingRateRatio) * WriteBlockSize) % CircularBufferSize;
				int SourceIndexEnd = ((int)((i + 1 ) * SamplingRateRatio) * WriteBlockSize) % CircularBufferSize;
				//let's make it mono
				int SampleSum = 0;
				int SampleCount = 0;
				for( int SourceIndex = SourceIndexStart; SourceIndex % CircularBufferSize <= SourceIndexEnd; SourceIndex += WriteBlockSize )
				{
					unsigned char *SourceData = &CircularBuffer[ ( SourceIndex % CircularBufferSize ) ];
					for( int SourceChannels = 0; SourceChannels < wfxWrite.nChannels; SourceChannels++ )
					{
						SampleSum += ( *(int*)&SourceData[ SourceChannels * WriteBytesPerChannel ] );
						SampleCount++;
					}
				}
				SampleSum = SampleSum / SampleCount;

				unsigned char *DestData = &Data[ i * ReadBlockSize ];
				for( int DestChannels = 0; DestChannels < wfxRead.nChannels; DestChannels++ )
					*(int*)&DestData[ DestChannels * ReadBytesPerChannel ] = SampleSum;	//wow, will this produce memory write error on last value ?
			}
		}
		ReadIndex = ( ReadIndex + TotalBufferSize ) % CircularBufferSize;

		TotalSamplesRead += TotalBufferSize;

		return S_OK;
	}
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
   HRESULT LoadData2( int BufferFrameCount, BYTE *Data, DWORD *flags )
	{
		*flags = 0;	// need to implement

		//it happens if "sleep" is too small
		if( BufferFrameCount == 0 || Data == NULL )
			return S_OK;

		int WriteBlockSize = wfxWrite.nChannels * wfxWrite.wBitsPerSample / 8;
		int ReadBlockSize = wfxRead.nChannels * wfxRead.wBitsPerSample / 8;

		float SamplingRateRatio = ( float )wfxWrite.nSamplesPerSec / ( float ) wfxRead.nSamplesPerSec;
		int	TotalBufferSize = ( (int)( SamplingRateRatio * BufferFrameCount * WriteBlockSize ) * 4 + 3 ) / 4;

		if( AudioConverter == NULL )
		{
			MMRESULT mmr;

			// find the biggest format size
			DWORD maxFormatSize = 0;
			mmr = acmMetrics( NULL, ACM_METRIC_MAX_SIZE_FORMAT, &maxFormatSize );

			mmr = acmStreamOpen( &AudioConverter,               // open an ACM conversion stream
					 NULL,                       // querying all ACM drivers
					 &wfxWrite,					 // from
					 &wfxRead,					 // to
					 NULL,                       // with no filter
					 0,                          // or async callbacks
					 0,                          // (and no data for the callback)
					 ACM_STREAMOPENF_NONREALTIME                           // flags
					 );

			//somebody said that sample rate must be the same. Pointless to go further ?
			if( mmr != MMSYSERR_NOERROR )
				return E_FAIL;

			// find out how big the decompressed buffer will be
			unsigned long rawbufsize = 0;
			mmr = acmStreamSize( AudioConverter, ReadBlockSize, &rawbufsize, ACM_STREAMSIZEF_DESTINATION );

			if( mmr != MMSYSERR_NOERROR || rawbufsize == 0 )
				return E_FAIL;

			// prepare the decoder
			ACMSTREAMHEADER ReadStreamHead;
			ZeroMemory( &ReadStreamHead, sizeof(ACMSTREAMHEADER ) );
			ReadStreamHead.cbStruct = sizeof(ACMSTREAMHEADER );
			ReadStreamHead.pbSrc = &CircularBuffer[ReadIndex];
			ReadStreamHead.cbSrcLength = CircularBufferSize - ReadIndex;
			ReadStreamHead.pbDst = Data;
			ReadStreamHead.cbDstLength = rawbufsize;
			mmr = acmStreamPrepareHeader( AudioConverter, &ReadStreamHead, 0 );
			if( mmr != MMSYSERR_NOERROR )
				return E_FAIL;

			// convert the data
			mmr = acmStreamConvert( AudioConverter, &ReadStreamHead, ACM_STREAMCONVERTF_BLOCKALIGN );
			if( mmr != MMSYSERR_NOERROR )
				return E_FAIL;

			mmr = acmStreamUnprepareHeader( AudioConverter, &ReadStreamHead, 0 );
			if( mmr != MMSYSERR_NOERROR )
				return E_FAIL;

    // write the decoded PCM to disk
//    count = fwrite( rawbuf, 1, ReadStreamHead.cbDstLengthUsed, fpOut );
		}

		TotalSamplesRead += TotalBufferSize;

		return S_OK;
	}
#endif

	unsigned char	*CircularBuffer;
	int				WriteIndex;
	int				ReadIndex;
	int				CacheDuration;
	int				CircularBufferSize;
	long			TotalSamplesStored;
	long			TotalSamplesRead;
	WAVEFORMATEX	wfxRead,wfxWrite;
	int				DebugForceStopRecordSeconds;
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
	HACMSTREAM		AudioConverter;
#endif
};

HRESULT RecordAudioStream(AudioBufferStore *pMySink)
{
    HRESULT hr;
//    REFERENCE_TIME hnsRequestedDuration = REFTIMES_PER_SEC;
    REFERENCE_TIME hnsActualDuration;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioCaptureClient *pCaptureClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 packetLength = 0;
    BOOL bDone = FALSE;
    BYTE *pData;
    DWORD flags;

	hr = CoInitialize( NULL );
    EXIT_ON_ERROR(hr)

    hr = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator );
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint( eCapture, eConsole, &pDevice );
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate( IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, AUDIO_SAMPLE_DURATION, 0, pwfx, NULL );
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService( IID_IAudioCaptureClient, (void**)&pCaptureClient );
    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
	hr = pMySink->SetWriteFormat(pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (bDone == FALSE)
    {
        // Sleep for half the buffer duration.
		// Calculate the actual duration of the allocated buffer.
		DWORD hnsActualDuration = 1000 * bufferFrameCount / pwfx->nSamplesPerSec;
        Sleep( hnsActualDuration / 2 );

        hr = pCaptureClient->GetNextPacketSize( &packetLength );
        EXIT_ON_ERROR(hr)

        while (packetLength != 0)
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer( &pData, &numFramesAvailable, &flags, NULL, NULL );
            EXIT_ON_ERROR(hr)

            if( flags & AUDCLNT_BUFFERFLAGS_SILENT )
            {
                pData = NULL;  // Tell CopyData to write silence.
            }

            // Copy the available capture data to the audio sink.
            hr = pMySink->CopyData( pData, numFramesAvailable, flags, &bDone );
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->ReleaseBuffer( numFramesAvailable );
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize( &packetLength );
            EXIT_ON_ERROR(hr)

//			printf( "write a new buffer of 1 second\n");
        }
    }

    hr = pAudioClient->Stop();  // Stop recording.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pCaptureClient)
	CoUninitialize();

    return hr;
}

HRESULT PlayAudioStream(AudioBufferStore *pMySource)
{
    HRESULT hr;
    IMMDeviceEnumerator *pEnumerator = NULL;
    IMMDevice *pDevice = NULL;
    IAudioClient *pAudioClient = NULL;
    IAudioRenderClient *pRenderClient = NULL;
    WAVEFORMATEX *pwfx = NULL;
    UINT32 bufferFrameCount;
    UINT32 numFramesAvailable;
    UINT32 numFramesPadding;
    BYTE *pData;
    DWORD flags = 0;

	hr = CoInitialize( NULL );
    EXIT_ON_ERROR(hr)

    hr = CoCreateInstance( CLSID_MMDeviceEnumerator, NULL, CLSCTX_ALL, IID_IMMDeviceEnumerator, (void**)&pEnumerator );
    EXIT_ON_ERROR(hr)

    hr = pEnumerator->GetDefaultAudioEndpoint( eRender, eConsole, &pDevice );
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate( IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient );
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, 0, AUDIO_SAMPLE_DURATION, 0, pwfx, NULL );
    EXIT_ON_ERROR(hr)

    // Tell the audio source which format to use.
    hr = pMySource->SetReadFormat(pwfx);
    EXIT_ON_ERROR(hr)

    // Get the actual size of the allocated buffer.
    hr = pAudioClient->GetBufferSize( &bufferFrameCount );
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService( IID_IAudioRenderClient, (void**)&pRenderClient );
    EXIT_ON_ERROR(hr)

    // Grab the entire buffer for the initial fill operation.
    hr = pRenderClient->GetBuffer( bufferFrameCount, &pData );
    EXIT_ON_ERROR(hr)

    // Load the initial data into the shared buffer.
    hr = pMySource->LoadData( bufferFrameCount, pData, &flags );
    EXIT_ON_ERROR(hr)

    hr = pRenderClient->ReleaseBuffer( bufferFrameCount, flags );
    EXIT_ON_ERROR(hr)

    // Calculate the actual duration of the allocated buffer.
//    hnsActualDuration = (double)REFTIMES_PER_SEC * bufferFrameCount / pwfx->nSamplesPerSec;

    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
    while (flags != AUDCLNT_BUFFERFLAGS_SILENT)
    {
        // Sleep for half the buffer duration.
//        Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));
	    DWORD hnsActualDuration = 1000 * bufferFrameCount / pwfx->nSamplesPerSec;
        Sleep( hnsActualDuration / 2 );

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr)

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr)

        // Get next 1/2-second of data from the audio source.
        hr = pMySource->LoadData(numFramesAvailable, pData, &flags);
        EXIT_ON_ERROR(hr)

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        EXIT_ON_ERROR(hr)

		if( bufferFrameCount > 0 )
			printf( "read a new buffer of 1 second\n");
    }

    // Wait for last data in buffer to play before stopping.
//    Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC/2));
//	Sleep((DWORD)(hnsActualDuration/REFTIMES_PER_MILLISEC));

    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)
	CoUninitialize();

    return hr;
}

void main()
{
	AudioBufferStore AudioStore( 10 );
	AudioStore.DebugForceStopRecordSeconds = 1;

	printf("Started record\n");
	RecordAudioStream( &AudioStore );
	printf("Done record\n");
	printf("Started playback\n");
	PlayAudioStream( &AudioStore );
	printf("Done playback\n");
}