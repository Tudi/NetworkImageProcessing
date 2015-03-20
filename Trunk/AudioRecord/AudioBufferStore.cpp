#include "StdAfx.h"

const CLSID CLSID_MMDeviceEnumerator = __uuidof(MMDeviceEnumerator);
const IID IID_IMMDeviceEnumerator = __uuidof(IMMDeviceEnumerator);
const IID IID_IAudioClient = __uuidof(IAudioClient);
const IID IID_IAudioCaptureClient = __uuidof(IAudioCaptureClient);
const IID IID_IAudioRenderClient = __uuidof(IAudioRenderClient);

AudioBufferStore::AudioBufferStore( )
{
	WriteIndex = 0;
	ReadIndex = 0;
	CircularBuffer = NULL;
	CacheDuration = AUDIO_SAMPLE_DURATION * 10;
	TotalBytesStored = 0;
	TotalBytesRead = 0;
	CircularBufferSize = 0;
	DebugForceStopRecordSeconds = 0;
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
	AudioConverter = NULL;
#endif
}

AudioBufferStore::~AudioBufferStore()
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

//use it before start
void AudioBufferStore::SetCacheDuration( int NewDuration )
{
	CacheDuration = NewDuration;
}

HRESULT AudioBufferStore::SetWriteFormat( WAVEFORMATEX *NewFormat )
{
	if( NewFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE )
		memcpy( &wfxWrite, NewFormat, sizeof( WAVEFORMATEXTENSIBLE ) );
	else
		memcpy( &wfxWrite, NewFormat, sizeof( WAVEFORMATEX ) );
	CircularBufferSize = wfxWrite.Format.nAvgBytesPerSec * CacheDuration;
	CircularBuffer = (unsigned char *)realloc( CircularBuffer, CircularBufferSize + wfxWrite.Format.nAvgBytesPerSec * 4 );
	return S_OK;
}

HRESULT AudioBufferStore::SetReadFormat( WAVEFORMATEX *NewFormat )
{
	if( NewFormat->wFormatTag == WAVE_FORMAT_EXTENSIBLE )
		memcpy( &wfxRead, NewFormat, sizeof( WAVEFORMATEXTENSIBLE ) );
	else
		memcpy( &wfxRead, NewFormat, sizeof( WAVEFORMATEX ) );
	return S_OK;
}

void AudioBufferStore::StoreData( unsigned char *Data, int ByteCount )
{
	if( ByteCount <= 0 || Data == NULL )
		return;
	if( WriteIndex + ByteCount <= CircularBufferSize )
	{
		memcpy( &CircularBuffer[WriteIndex], Data, ByteCount );
		WriteIndex += ByteCount;
	}
	else
	{
		int CanCopyToEnd = CircularBufferSize - WriteIndex;
		if( CanCopyToEnd > 0 )
			memcpy( &CircularBuffer[WriteIndex], &Data[0], CanCopyToEnd );
		int CanCopyBeggining = ByteCount - CanCopyToEnd;
		if( CanCopyBeggining > CircularBufferSize )
			CanCopyBeggining = CircularBufferSize;
		if( CanCopyBeggining > 0 )
			memcpy( &CircularBuffer[0], &Data[CanCopyToEnd], CanCopyBeggining );
		WriteIndex = CanCopyBeggining;
	}

	TotalBytesStored += ByteCount;
}

HRESULT AudioBufferStore::StoreData( unsigned char *Data, int FrameCount, DWORD Flags, int *done )
{
	int	TotalBufferSize = FrameCount * wfxWrite.Format.nBlockAlign;

	StoreData( Data, TotalBufferSize );

	//just testing. Remove me later
	if( DebugForceStopRecordSeconds != 0 && TotalBytesStored > DebugForceStopRecordSeconds * (int)wfxWrite.Format.nAvgBytesPerSec )
		return E_FAIL;

	return S_OK;
}

HRESULT AudioBufferStore::LoadData( int BufferFrameCount, BYTE *Data, DWORD *flags )
{
	*flags = 0;	// need to implement

	//it happens if "sleep" is too small
	if( BufferFrameCount == 0 )
		return S_OK;

	int ReadBlockSize = wfxRead.Format.nBlockAlign;
	int WriteBlockSize = wfxWrite.Format.nBlockAlign;
	// need to convert sampling rate
	float SamplingRateRatio = ( float )wfxWrite.Format.nSamplesPerSec / ( float ) wfxRead.Format.nSamplesPerSec;
	int	TotalBufferSize = ( (int)( SamplingRateRatio * BufferFrameCount * WriteBlockSize ) * 4 + 3 ) / 4;

	if( TotalBytesRead + TotalBufferSize > TotalBytesStored )
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

	TotalBytesRead += TotalBufferSize;

	return S_OK;
}
#ifdef SUPPORT_FOR_OS_AUDIO_CONVERSION
HRESULT AudioBufferStore::LoadData2( int BufferFrameCount, BYTE *Data, DWORD *flags )
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

	TotalBytesRead += TotalBufferSize;

	return S_OK;
}
#endif

int	AudioBufferStore::GetRequiredNetworkBufferSize() 
{
	return (TotalBytesStored - TotalBytesRead) % CircularBufferSize + sizeof( WAVEFORMATEXTENSIBLE );
}

int AudioBufferStore::GetNetworkPacket( unsigned char *buff, int BuffSize )
{
	if( (TotalBytesStored - TotalBytesRead) <= 0 )
		return 0;

	int HeaderSize = sizeof( NetworkPacketHeader ) + sizeof( WAVEFORMATEXTENSIBLE );

	if( TotalBytesStored - TotalBytesRead > CircularBufferSize )
		TotalBytesRead = TotalBytesStored - CircularBufferSize;

	int MaxBytesCanWrite = ( TotalBytesStored - TotalBytesRead );
	if( BuffSize < MaxBytesCanWrite )
		MaxBytesCanWrite = ( BuffSize - HeaderSize ) / wfxWrite.Format.nBlockAlign * wfxWrite.Format.nBlockAlign;

	//write headers
	NetworkPacketHeader *NH = (NetworkPacketHeader *)&buff[0];
	memset( NH, 0, sizeof( NetworkPacketHeader ) );
	NH->PacketSize = HeaderSize + MaxBytesCanWrite;

	memcpy( &buff[ sizeof( NetworkPacketHeader ) ], &wfxWrite, sizeof( WAVEFORMATEXTENSIBLE ) );

	//write content
	if( ReadIndex + MaxBytesCanWrite <= CircularBufferSize )
	{
		memcpy( &buff[HeaderSize], &CircularBuffer[ReadIndex], MaxBytesCanWrite );
	}
	else
	{
		int CanCopyToEnd = CircularBufferSize - ReadIndex;
		if( CanCopyToEnd > 0 )
			memcpy( &buff[HeaderSize], &CircularBuffer[ReadIndex], CanCopyToEnd );
		int CanCopyBeggining = MaxBytesCanWrite - CanCopyToEnd;
		if( CanCopyBeggining > CircularBufferSize )
			CanCopyBeggining = CircularBufferSize;
		if( CanCopyBeggining > 0 )
			memcpy( &buff[HeaderSize + CanCopyToEnd], &CircularBuffer[0], CanCopyBeggining );
	} 

	ReadIndex = ( ReadIndex + MaxBytesCanWrite ) % CircularBufferSize;
	TotalBytesRead += MaxBytesCanWrite;

	return ( HeaderSize + MaxBytesCanWrite );
}

void AudioBufferStore::StoreNetworkData( unsigned char *Data, int size )
{
	if( size <= 0 )
		return;
	int HeaderSize = sizeof( NetworkPacketHeader ) + sizeof( WAVEFORMATEXTENSIBLE );
	SetWriteFormat( (WAVEFORMATEX*) &Data[ sizeof( NetworkPacketHeader ) ] );
	int DataSize = size - HeaderSize;
	if( DataSize > 0 )
		StoreData( &Data[HeaderSize], DataSize );
}