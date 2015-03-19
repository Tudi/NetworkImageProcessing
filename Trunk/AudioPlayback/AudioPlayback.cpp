#include "StdAfx.h"

void PlayAudioStream( void *arg )
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
    hr = AudioStore.SetReadFormat(pwfx);
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
    hr = AudioStore.LoadData( bufferFrameCount, pData, &flags );
    EXIT_ON_ERROR(hr)

    hr = pRenderClient->ReleaseBuffer( bufferFrameCount, flags );
    EXIT_ON_ERROR(hr)

    hr = pAudioClient->Start();  // Start playing.
    EXIT_ON_ERROR(hr)

    // Each loop fills about half of the shared buffer.
	GlobalData.ThreadsAliveCount++;
    while( 
//		flags != AUDCLNT_BUFFERFLAGS_SILENT
		GlobalData.ThreadIsRunning == 1
		)
    {
        // Sleep for half the buffer duration.
	    DWORD hnsActualDuration = 1000 * bufferFrameCount / pwfx->nSamplesPerSec;
        Sleep( hnsActualDuration / 2 );

        // See how much buffer space is available.
        hr = pAudioClient->GetCurrentPadding(&numFramesPadding);
        EXIT_ON_ERROR(hr)

        numFramesAvailable = bufferFrameCount - numFramesPadding;

        // Grab all the available space in the shared buffer.
        hr = pRenderClient->GetBuffer(numFramesAvailable, &pData);
        EXIT_ON_ERROR(hr)

        hr = AudioStore.LoadData(numFramesAvailable, pData, &flags);
        EXIT_ON_ERROR(hr)

        hr = pRenderClient->ReleaseBuffer(numFramesAvailable, flags);
        EXIT_ON_ERROR(hr)

//		if( bufferFrameCount > 0 )
//			printf( "read a new buffer of 1 second\n");
    }

    hr = pAudioClient->Stop();  // Stop playing.
    EXIT_ON_ERROR(hr)

Exit:
    CoTaskMemFree(pwfx);
    SAFE_RELEASE(pEnumerator)
    SAFE_RELEASE(pDevice)
    SAFE_RELEASE(pAudioClient)
    SAFE_RELEASE(pRenderClient)
	CoUninitialize();
	GlobalData.ThreadsAliveCount--;
}