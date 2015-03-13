#include "StdAfx.h"

// Get the horizontal and vertical screen sizes in pixel
void GetDesktopResolution(int& horizontal, int& vertical)
{
   RECT desktop;
   // Get a handle to the desktop window
   const HWND hDesktop = GetDesktopWindow();
   // Get the size of screen to the variable desktop
   GetWindowRect(hDesktop, &desktop);
   // The top left corner will have coordinates (0,0)
   // and the bottom right corner will have coordinates
   // (horizontal, vertical)
   horizontal = desktop.right;
   vertical = desktop.bottom;
}

void BarbaricTCharToChar( TCHAR *In, char *Out, int MaxLen )
{
	for( int i=0; i<MaxLen && *In != 0; i++ )
	{
		*Out = (char)*In;
		Out++;
		In++;
	}
	*Out = '\0';
}

struct SearchCallbackIOStruct
{
	char *SearchName;
	HWND ret;
	bool SearchDone;
};


//non optimized bad version of strstr case insensitive. Remind me to rewrite this
char *stristr( char *SearchIn, char *SearchWhat )
{
	for( int a=0;SearchIn[a]!=0 && a < 10000;a++)
	{
		int b;
		for( b=0;SearchWhat[b]!=0 && SearchIn[a+b]!=0 && tolower( SearchWhat[b] ) == tolower( SearchIn[a+b] ) && b < 10000; b++ );
		if( SearchWhat[b] == 0 )
			return &SearchIn[a];
	}
	return NULL;
}

BOOL CALLBACK EnumWindowsProcFindWndByName( HWND hwnd, LPARAM lParam )
{
	char WindowTitle[ DEFAULT_BUFLEN ];
	SearchCallbackIOStruct *pIO = (SearchCallbackIOStruct*)lParam;

	if( pIO->SearchDone == true )
		return FALSE;

	//only search on main window names. If you need window in window search than enable this
//	if( GetParent( hwnd ) != 0 )
//		return FALSE;

	GetWindowText( hwnd, WindowTitle, sizeof( WindowTitle ) );

//	if( strstr( WindowTitle, pIO->SearchName ) != NULL )
	if( stristr( WindowTitle, pIO->SearchName ) != NULL )
	{
		pIO->ret = hwnd;
		pIO->SearchDone = true;
	}
	return TRUE;
}

SearchCallbackIOStruct MySearch;
HWND FindWindowWithNameNonThreaded( char *Name )
{
	if( Name == NULL )
		return 0;

	MySearch.ret = 0; // mark that we did not find it yet
	MySearch.SearchName = Name;
	MySearch.SearchDone = false;

	EnumWindows( EnumWindowsProcFindWndByName, (LPARAM)&MySearch );
	int WaitTimeout = 1000;
	while( MySearch.SearchDone == false && WaitTimeout > 0 )
	{
		WaitTimeout -= 100;
		Sleep( 100 );
	}

	if( WaitTimeout <= 0 )
		printf( "searching for window with title %s timed out !\n", Name );

	return MySearch.ret;
}

ProcessNameList::~ProcessNameList()
{
	for( std::list<char*>::iterator itr = Names.begin(); itr != Names.end(); itr++ )
		free( *itr );
	Names.clear();
}

BOOL CALLBACK EnumWindowsProc( HWND hwnd, LPARAM lParam )
{
	char WindowTitle[ DEFAULT_BUFLEN ];

    if( hwnd == GetShellWindow() )
        return false;

	//only search on main window names. If you need window in window search than enable this
//	HWND hwndroot = GetAncestor(hwnd, GA_ROOTOWNER);
//	if( 
//		GetWindow( hwnd, GW_OWNER ) == 0 
//		|| 
//		GetAncestor( hwnd, GA_PARENT ) 
//		IsAltTabWindow( hwnd ) == FALSE
//		hwndroot == GetLastVisibleActivePopUpOfWindow( hwndroot )
//		hwndroot != GetShellWindow()
//		hwnd != hwndroot
//		)
//		return TRUE;
 
	if( IsWindowVisible( hwnd ) == FALSE )
		return TRUE;

//	RECT WindowSize;
//	GetClientRect( hwnd, &WindowSize );
//	if( WindowSize.right <= 32 || WindowSize.bottom <= 32 )
//		return true;

	GetWindowText( hwnd, WindowTitle, sizeof( WindowTitle ) );

	//window without a name
	if( WindowTitle[0] == 0 )
		return TRUE;

	ProcessNameList *pIO = (ProcessNameList*)lParam;

	pIO->Count++;
	pIO->Names.push_back( _strdup( WindowTitle ) );

	return TRUE;
}

void GetProcessNameList( ProcessNameList *Ret )
{
	Ret->Count = 0;
	Ret->Names.clear();
	EnumWindows( EnumWindowsProc, (LPARAM)Ret );
}

void ShortTermDataUsageSampler::AddSample( unsigned int val )
{
	if( HistoryDuration == 0 )
		return;
	SampleStore *NewSample = (SampleStore *)malloc( sizeof( SampleStore ) );
	NewSample->Value = val;
	NewSample->Stamp = GetTimer();
	Samples.push_front( NewSample );
}

unsigned int ShortTermDataUsageSampler::GetSumInterval()
{
	if( HistoryDuration == 0 )
		return 0;
	unsigned int Sum = 0;
//	unsigned int SampleCount = 0;
	unsigned int StampNow = GetTimer();
	unsigned int StampLimit = StampNow - HistoryDuration;
	std::list<SampleStore*>::iterator itr1,itr2;
	for( itr1 = Samples.begin(); itr1 != Samples.end(); itr1++ )
	{
		SampleStore *Sample = *itr1;
		if( Sample->Stamp >= StampLimit )
		{
			Sum += Sample->Value;
//			SampleCount++;
		}
		else
			break;
	}
	// will broably delete last value in most cases
	for( ; itr1 != Samples.end(); )
	{
		itr2 = itr1;
		itr1++;
		SampleStore *Sample = *itr2;
		if( Sample->Stamp < StampLimit )
		{
			Samples.erase( itr2 );
			free( Sample );
		}
	}
//	if( SampleCount > 0 )
//		return ( Sum / SampleCount );
	return Sum;
}

#if 0
BOOL IsAltTabWindow(HWND hwnd)
{
	// Start at the root owner
	HWND hwndWalk = GetAncestor(hwnd, GA_ROOTOWNER);

	// See if we are the last active visible popup
	HWND hwndTry;
	while( (hwndTry = GetLastActivePopup( hwndWalk )) != hwndTry )
	{
		if( IsWindowVisible( hwndTry ) ) 
			break;
		hwndWalk = hwndTry;
	}
	return hwndWalk == hwnd;
}

HWND GetLastVisibleActivePopUpOfWindow(HWND window)
{
    HWND lastPopUp = GetLastActivePopup( window );
    if( IsWindowVisible( lastPopUp ) )
        return lastPopUp;
    else if( lastPopUp == window)
        return NULL;
    else
        return GetLastVisibleActivePopUpOfWindow( lastPopUp );
}

#include <tlhelp32.h>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")
char *GetProcessName( DWORD processID )
{
    TCHAR szProcessName[MAX_PATH] = TEXT(" ");
    HANDLE hProcess = OpenProcess( PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );
    if( NULL != hProcess )
    {
        HMODULE hMod;
        DWORD cbNeeded;
        if ( EnumProcessModules( hProcess, &hMod, sizeof(hMod), &cbNeeded) )
            GetModuleBaseName( hProcess, hMod, szProcessName, sizeof(szProcessName)/sizeof(TCHAR) );
	    CloseHandle( hProcess );
		return _strdup( szProcessName );
    }
    CloseHandle( hProcess );
	return NULL;
}

void GetProcessNameList( ProcessNameList *Ret )
{
	HANDLE hProcessSnap;
	PROCESSENTRY32 pe32;

	// Take a snapshot of all processes in the system.
	hProcessSnap = CreateToolhelp32Snapshot( TH32CS_SNAPPROCESS, 0 );
	if( hProcessSnap == INVALID_HANDLE_VALUE )
	{
//		printf( TEXT("CreateToolhelp32Snapshot (of processes)") );
		return;
	}

	// Set the size of the structure before using it.
	pe32.dwSize = sizeof( PROCESSENTRY32 );
	if( !Process32First( hProcessSnap, &pe32 ) )
	{
//		printf( TEXT("Process32First") ); // show cause of failure
		CloseHandle( hProcessSnap );          // clean the snapshot object
		return;
	}

	do
	{
		Ret->Count++;
		char *Name = GetProcessName( pe32.th32ProcessID );
		if( Name != NULL )
		{
			if( Name[0] == ' ' )
			{
				Ret->Names.push_back( _strdup( pe32.szExeFile ) );
				free( Name );
			}
			else
				Ret->Names.push_back( Name );
		}
	} while( Process32Next( hProcessSnap, &pe32 ) );

	CloseHandle( hProcessSnap );
	return;
}
#endif