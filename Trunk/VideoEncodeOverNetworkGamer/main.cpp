#include "StdAfx.h"
#include <conio.h>

GlobalStore GlobalData;

#if defined( CAN_USE_ATL_IMG ) && !defined( LIB_BUILD )
void main()
{
	//load the settings from the ini file
	LoadSettingsFromFile( NULL );

	InitDataProcessing();

	StartDataProcessing();

	char c;
	do {
		c = _getch();
	}while( c != 'e' && GlobalData.ThreadIsRunning != 0 );

	//wait for thread to shut down
	ShutDownAllDataProcessing( 1 );

	printf("Shut down complete\n");
}
#endif