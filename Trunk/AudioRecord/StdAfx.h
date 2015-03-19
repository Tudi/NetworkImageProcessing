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
#include "../VideoEncodeOverNetworkGamer/ServerNetwork.h"

#include "AudioRecord.h"
#include "AudioPlayback.h"
#include "DataSender.h"