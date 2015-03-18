#pragma once

class CScreenImage;

struct GlobalStore
{
	HWND			WndSrc;
	char			*WindowName;
	CScreenImage	*CapturedScreen;
	int				ResizeWidth,ResizeHeight;
	int				ResizeMethod;
	int				ColorFilterMask;
	int				CompressionStrength;
	int				FPSLimit;
	int				ShowStatistics;
	char			*VideoNetworkIP;
	char			*VideoNetworkPort;
//	int				MaxPacketSize;
	int				ThreadIsRunning;
	int				IncludeWindowBorder;
	int				StartX,StartY,ForcedWidth,ForcedHeight;
	int				StatAvgNetworkUsage, StatMaxPossibleFPS, StatAvgFPS;
	float			StatLastCPUUsage;
	char			*SoundNetworkPort;
};

extern GlobalStore GlobalData;

#define DEFAULT_BUFLEN							200
#define RGB_BYTE_COUNT							4	//not safe to macro this as default RGB byte count is actually 3 bytes
#define MEMORY_SAFEGUARD_INTRINSIC_IMPLEMENT	128
#define AUDIO_SAMPLE_DURATION					100*10000	

enum WORKERTHREADSTATES
{
	WT_WAITING_TO_START = 0,
	WT_RUNNING			= 1,
	WT_WAITING_TO_STOP	= 2,
	WT_STOPPED			= 3,
};