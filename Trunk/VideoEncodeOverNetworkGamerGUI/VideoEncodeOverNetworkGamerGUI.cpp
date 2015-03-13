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

	InitDefaultWorkerThreadSettings();

	// Create the main window and run it
	Application::Run( gcnew Form1() );

	ShutDownNetworkThreads();

	return 0;
}
