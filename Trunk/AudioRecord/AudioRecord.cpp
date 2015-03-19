#include "StdAfx.h"

void RecordAudioStream( void *arg )
{
    HRESULT hr;
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

    hr = pEnumerator->GetDefaultAudioEndpoint( eRender, eConsole, &pDevice );
    EXIT_ON_ERROR(hr)

    hr = pDevice->Activate( IID_IAudioClient, CLSCTX_ALL, NULL, (void**)&pAudioClient);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetMixFormat(&pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Initialize( AUDCLNT_SHAREMODE_SHARED, AUDCLNT_STREAMFLAGS_LOOPBACK, AUDIO_SAMPLE_DURATION, 0, pwfx, NULL );
    EXIT_ON_ERROR(hr)

    // Get the size of the allocated buffer.
    hr = pAudioClient->GetBufferSize(&bufferFrameCount);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->GetService( IID_IAudioCaptureClient, (void**)&pCaptureClient );
    EXIT_ON_ERROR(hr)

    // Notify the audio sink which format to use.
	hr = AudioStore.SetWriteFormat(pwfx);
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Start();  // Start recording.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
	int LoopCounter = 0;
	int ErrorCounter = 0;
	GlobalData.ThreadsAliveCount++;
	while( bDone == FALSE && GlobalData.ThreadIsRunning == 1 )
    {
        // Sleep for half the buffer duration.
		// Calculate the actual duration of the allocated buffer.
		DWORD hnsActualDuration = 1000 * bufferFrameCount / pwfx->nSamplesPerSec;
        Sleep( hnsActualDuration / 2 );

        hr = pCaptureClient->GetNextPacketSize( &packetLength );
        EXIT_ON_ERROR(hr)

        while( packetLength != 0 )
        {
            // Get the available data in the shared buffer.
            hr = pCaptureClient->GetBuffer( &pData, &numFramesAvailable, &flags, NULL, NULL );
            EXIT_ON_ERROR(hr)

            if( flags & AUDCLNT_BUFFERFLAGS_SILENT )
            {
                pData = NULL;  // Tell CopyData to write silence.
            }
			if( flags & AUDCLNT_BUFFERFLAGS_DATA_DISCONTINUITY )
				ErrorCounter++;

            // Copy the available capture data to the audio sink.
            hr = AudioStore.StoreData( pData, numFramesAvailable, flags, &bDone );
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->ReleaseBuffer( numFramesAvailable );
            EXIT_ON_ERROR(hr)

            hr = pCaptureClient->GetNextPacketSize( &packetLength );
            EXIT_ON_ERROR(hr)

//			printf( "write a new buffer of x msecond. index %d Err %d size %d\n", LoopCounter++, ErrorCounter, numFramesAvailable );
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

	GlobalData.ThreadsAliveCount--;
}