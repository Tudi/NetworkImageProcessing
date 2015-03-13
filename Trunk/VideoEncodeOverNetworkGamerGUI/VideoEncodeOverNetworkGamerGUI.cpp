// VideoEncodeOverNetworkGamerGUI.cpp : main project file.

#include "stdafx.h"
#include "Form1.h"

using namespace VideoEncodeOverNetworkGamerGUI;

[STAThreadAttribute]
int main(array<System::String ^> ^args)
{
	// Enabling Windows XP visual effects before any controls are created
	Application::EnableVisualStyles();
	Application::SetCompatibleTextRenderingDefault(false); 

	memset( &GlobalData, 0, sizeof( GlobalData ) );

	// Create the main window and run it
	Application::Run( gcnew Form1() );

	ShutDownAllDataProcessing();

	return 0;
}
