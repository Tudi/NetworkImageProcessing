#include "StdAfx.h"

GlobalStore GlobalData;

void main()
{
	AudioBufferStore AudioStore( 10 );
	AudioStore.DebugForceStopRecordSeconds = 20;

	printf("Started record\n");
	RecordAudioStream( &AudioStore );
	printf("Done record\n");
	printf("Started playback\n");
	PlayAudioStream( &AudioStore );
	printf("Done playback\n");
}