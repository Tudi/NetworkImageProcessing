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
		if( NewFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE )
			memcpy( &wfxWrite, NewFormat, sizeof( WAVEFORMATEXTENSIBLE ) );
		else
			memcpy( &wfxWrite, NewFormat, sizeof( WAVEFORMATEX ) );
		CircularBufferSize = wfxWrite.Format.nAvgBytesPerSec * CacheDuration;
		CircularBuffer = (unsigned char *)realloc( CircularBuffer, CircularBufferSize + wfxWrite.Format.nAvgBytesPerSec * 4 );
		return S_OK;
	}
	HRESULT SetReadFormat( WAVEFORMATEX *NewFormat )
	{
		if( NewFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE )
			memcpy( &wfxRead, NewFormat, sizeof( WAVEFORMATEXTENSIBLE ) );
		else
			memcpy( &wfxRead, NewFormat, sizeof( WAVEFORMATEX ) );
		return S_OK;
	}
	HRESULT CopyData( unsigned char *Data, int FrameCount, DWORD Flags, int *done )
	{
		int WriteBlockSize = wfxWrite.Format.nChannels * wfxWrite.Format.wBitsPerSample / 8;
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
		if( DebugForceStopRecordSeconds != 0 && TotalSamplesStored > DebugForceStopRecordSeconds * (int)wfxWrite.Format.nAvgBytesPerSec )
			return E_FAIL;

		return S_OK;
	}
    HRESULT LoadData( int BufferFrameCount, BYTE *Data, DWORD *flags )
	{
		*flags = 0;	// need to implement

		//it happens if "sleep" is too small
		if( BufferFrameCount == 0 )
			return S_OK;

		int ReadBlockSize = wfxRead.Format.nChannels * wfxRead.Format.wBitsPerSample / 8;
		int WriteBlockSize = wfxWrite.Format.nChannels * wfxWrite.Format.wBitsPerSample / 8;
		// need to convert sampling rate
		float SamplingRateRatio = ( float )wfxWrite.Format.nSamplesPerSec / ( float ) wfxRead.Format.nSamplesPerSec;
		int	TotalBufferSize = ( (int)( SamplingRateRatio * BufferFrameCount * WriteBlockSize ) * 4 + 3 ) / 4;

		if( TotalSamplesRead + TotalBufferSize > TotalSamplesStored )
			return E_FAIL;

		int IsBothFloatFormat = 0;
		int IsBothPCMFormat = 0;
		if( wfxWrite.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE && wfxRead.Format.wFormatTag == WAVE_FORMAT_EXTENSIBLE )
		{
			IsBothFloatFormat = IsEqualGUID( wfxRead.SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT ) && IsEqualGUID( wfxWrite.SubFormat, KSDATAFORMAT_SUBTYPE_IEEE_FLOAT );
			IsBothPCMFormat = IsEqualGUID( wfxRead.SubFormat, KSDATAFORMAT_SUBTYPE_PCM ) && IsEqualGUID( wfxWrite.SubFormat, KSDATAFORMAT_SUBTYPE_PCM );
		}
		else if( wfxWrite.Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT && wfxRead.Format.wFormatTag == WAVE_FORMAT_IEEE_FLOAT )
			IsBothFloatFormat = 1;
		else if( wfxWrite.Format.wFormatTag == WAVE_FORMAT_PCM && wfxRead.Format.wFormatTag == WAVE_FORMAT_PCM )
			IsBothPCMFormat = 1;
		//pray to the lord jezuz that we guess it. Else expect some noise :(
		if( IsBothPCMFormat == 0 && IsBothFloatFormat == 0 )
			IsBothFloatFormat = 1;

		if( wfxRead.Format.nChannels == wfxWrite.Format.nChannels 
			&& wfxRead.Format.wBitsPerSample == wfxWrite.Format.wBitsPerSample 
			&& wfxRead.Format.nSamplesPerSec == wfxWrite.Format.nSamplesPerSec 
			&& wfxRead.Format.wFormatTag == wfxWrite.Format.wFormatTag )
		{
			if( ReadIndex + TotalBufferSize <= CircularBufferSize )
			{
				memcpy( &Data[0], &CircularBuffer[ReadIndex], TotalBufferSize );
			}
			else
			{
				int CanCopyToEnd = CircularBufferSize - ReadIndex;
				if( CanCopyToEnd > 0 )
					memcpy( &Data[0], &CircularBuffer[ReadIndex], CanCopyToEnd );
				int CanCopyBeggining = TotalBufferSize - CanCopyToEnd;
				if( CanCopyBeggining > 0 )
					memcpy( &Data[CanCopyToEnd], &CircularBuffer[0], CanCopyBeggining );
			} 
		}
		else if( IsBothFloatFormat == 1 )
		{
			// need to convert channel count
			int WriteBytesPerChannel = wfxWrite.Format.wBitsPerSample / 8;
			int ReadBytesPerChannel = wfxRead.Format.wBitsPerSample / 8;
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
					for( int SourceChannels = 0; SourceChannels < wfxWrite.Format.nChannels; SourceChannels++ )
					{
						SampleSum += ( *(float*)&SourceData[ SourceChannels * WriteBytesPerChannel ] );
						SampleCount++;
					}
				}
				SampleSum = SampleSum / SampleCount;

				unsigned char *DestData = &Data[ i * ReadBlockSize ];
				for( int DestChannels = 0; DestChannels < wfxRead.Format.nChannels; DestChannels++ )
					*(float*)&DestData[ DestChannels * ReadBytesPerChannel ] = SampleSum;	//wow, will this produce memory write error on last value ?
			}
		}
		else if( IsBothPCMFormat == 1 && wfxRead.Format.wBitsPerSample == wfxWrite.Format.wBitsPerSample && wfxRead.Format.wBitsPerSample == 32 )
		{
			// need to convert channel count
			int WriteBytesPerChannel = wfxWrite.Format.wBitsPerSample / 8;
			int ReadBytesPerChannel = wfxRead.Format.wBitsPerSample / 8;
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
					for( int SourceChannels = 0; SourceChannels < wfxWrite.Format.nChannels; SourceChannels++ )
					{
						SampleSum += ( *(int*)&SourceData[ SourceChannels * WriteBytesPerChannel ] );
						SampleCount++;
					}
				}
				SampleSum = SampleSum / SampleCount;

				unsigned char *DestData = &Data[ i * ReadBlockSize ];
				for( int DestChannels = 0; DestChannels < wfxRead.Format.nChannels; DestChannels++ )
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

		int WriteBlockSize = wfxWrite.Format.nChannels * wfxWrite.Format.wBitsPerSample / 8;
		int ReadBlockSize = wfxRead.Format.nChannels * wfxRead.Format.wBitsPerSample / 8;

		float SamplingRateRatio = ( float )wfxWrite.Format.nSamplesPerSec / ( float ) wfxRead.Format.nSamplesPerSec;
		int	TotalBufferSize = ( (int)( SamplingRateRatio * BufferFrameCount * WriteBlockSize ) * 4 + 3 ) / 4;

		if( AudioConverter == NULL )
		{
			MMRESULT mmr;

			// find the biggest format size
			DWORD maxFormatSize = 0;
			mmr = acmMetrics( NULL, ACM_METRIC_MAX_SIZE_FORMAT, &maxFormatSize );

			mmr = acmStreamOpen( &AudioConverter,               // open an ACM conversion stream
					 NULL,                       // querying all ACM drivers
					 (WAVEFORMATEX*)&wfxWrite,					 // from
					 (WAVEFORMATEX*)&wfxRead,					 // to
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

	unsigned char			*CircularBuffer;
	int						WriteIndex;
	int						ReadIndex;
	int						CacheDuration;
	int						CircularBufferSize;
	long					TotalSamplesStored;
	long					TotalSamplesRead;
	WAVEFORMATEXTENSIBLE	wfxRead,wfxWrite;
	int						DebugForceStopRecordSeconds;
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
	HACMSTREAM				AudioConverter;
#endif
};
