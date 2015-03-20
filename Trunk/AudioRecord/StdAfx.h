#pragma once

#include <windows.h>
#include <stdio.h>
#include <mmsystem.h>
#include <audioclient.h>
#include <avrt.h>
#include <mmdeviceapi.h>
#include <list>
#include <map>
#include <process.h>

#include "AudioBufferStore.h"
#include "../VideoEncodeOverNetworkGamer/GamerPCDefines.h"
#include "../VideoEncodeOverNetworkGamer/Tools.h"
#include "../VideoEncodeOverNetworkGamer/NetworkPackets.h"
#include "../VideoEncodeOverNetworkGamer/ServerNetwork.h"
#include "../VideoEncodeOverNetworkGamer/IniReader.h"

#include "AudioRecord.h"
#include "../AudioPlayback/AudioPlayback.h"
#include "DataSender.h"
#include "AudioDataProcessing.h"