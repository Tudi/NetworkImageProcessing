// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <stdio.h>
#include <process.h>
#include <atlimage.h>
#include <assert.h>

#include "../VideoEncodeOverNetworkGamer/NetworkPackets.h"
#include "../VideoEncodeOverNetworkGamer/GamerPCDefines.h"
#include "../VideoEncodeOverNetworkGamer/IniReader.h"
#include "../VideoEncodeOverNetworkGamer/PreciseTimer.h"
#include "../VideoEncodeOverNetworkGamer/zlib.h"
#include "ClientNetwork.h"
#include "../AudioPlayback/AudioDataProcessing.h"

void MyMain( HWND MainWindow );

extern GlobalStore GlobalData;

#define RGB_BYTE_COUNT				4
#define MAX_RESOLUTION_SUPPORTED	4000*4000	//wow, so hacky, we should dynamically reallocate the network buffer based on requirements