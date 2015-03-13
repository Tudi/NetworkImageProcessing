#pragma once

//wow, people still do this in 2015 ? Burn it with fire !
//just to kill your curiosity. This hack is temp added because you can resize input window size
#define VERY_LARGE_RESOLUTION	( 4000 * 4000 )

int InitDataProcessing();

void StartDataProcessing();

void ShutDownAllDataProcessing( int ShutdownNetwork = 0 );