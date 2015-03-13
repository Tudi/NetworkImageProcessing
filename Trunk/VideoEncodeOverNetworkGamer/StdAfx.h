// stdafx.h : include file for standard system include files,
// or project specific include files that are used frequently, but
// are changed infrequently
//

#pragma once

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files:
#include <windows.h>

// C RunTime Header Files
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
//#include <tchar.h>

#define CAN_USE_ATL_IMG

// TODO: reference additional headers your program requires here
#include <stdio.h>
#include <string.h>
#include <assert.h>
#ifdef CAN_USE_ATL_IMG
	#include <atlimage.h>
#else
	#include <atltypes.h>
#endif
#include <assert.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <process.h>

#include <map>

#include "GamerPCDefines.h"

#include "ScreenCap.h"
#include "Resampling.h"
#include "IniReader.h"
#include "Tools.h"
#include "PreciseTimer.h"
//#include "ImageTools.h"
#include "zlib.h"
#include "ServerNetwork.h"
#include "systemMetrics.h"