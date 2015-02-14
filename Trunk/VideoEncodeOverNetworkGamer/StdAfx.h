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


// TODO: reference additional headers your program requires here
#include <windows.h>
#include <stdio.h>
//#include <fcntl.h>
//#include <io.h>
//#include <iostream>
//#include <fstream>
#include <string.h>
#include <assert.h>
//#include <tmmintrin.h>
#include <conio.h>
#include <atlimage.h>
#include <assert.h>

#include "ScreenCap.h"
#include "Resampling.h"
#include "IniReader.h"
#include "Tools.h"
#include "PreciseTimer.h"
//#include "ImageTools.h"
#include "zlib.h"
#include "ServerNetwork.h"

struct GlobalStore
{
	HWND			WndSrc;
	char			*WindowName;
	CScreenImage	*CapturedScreen;
	int				ResizeWidth,ResizeHeight;
	int				ResizeMethod;
	int				CompressionStrength;
	int				FPSLimit;
	int				ShowStatistics;
	char			*EncoderIP;
	char			*EncoderPort;
//	int				MaxPacketSize;
	int				ThreadIsRunning;
};

extern GlobalStore GlobalData;

#define DEFAULT_BUFLEN 200
#define RGB_BYTE_COUNT 4	//not safe to macro this as default RGB byte count is actually 3 bytes