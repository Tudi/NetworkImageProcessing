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

BOOL CALLBACK EnumWindowsProcFindWndByName( HWND hwnd, LPARAM lParam )
{
	char WindowTitle[ DEFAULT_BUFLEN ];
	SearchCallbackIOStruct *pIO = (SearchCallbackIOStruct*)lParam;

	if( pIO->SearchDone == true )
		return FALSE;

	GetWindowText( hwnd, WindowTitle, sizeof( WindowTitle ) );

	if( strstr( WindowTitle, pIO->SearchName ) != NULL )
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
