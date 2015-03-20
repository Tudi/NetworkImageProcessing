#pragma once

// Networking libraries
#include <Windows.h>
//#include <winsock2.h>
//#include <ws2tcpip.h>
#include <stdio.h> 
#include <mmsystem.h>
#include <audioclient.h>
#include <avrt.h>
#include <mmdeviceapi.h>
#include <list>
#include <map>
#include <process.h>

#include "../VideoEncodeOverNetworkGamer/GamerPCDefines.h"
#include "../AudioRecord/AudioBufferStore.h"
#include "../VideoEncodeOverNetworkEncoder/ClientNetwork.h"
#include "../VideoEncodeOverNetworkGamer/IniReader.h"
//#include "../AudioRecord/AudioPlayback.h"
#include "AudioPlayback.h"
#include "DataReader.h"
#include "AudioDataProcessing.h"

extern AudioBufferStore	AudioStore;