#ifndef _TOOLS_H_
#define _TOOLS_H_

void GetDesktopResolution(int& horizontal, int& vertical);
void BarbaricTCharToChar( TCHAR *In, char *Out, int MaxLen );
HWND FindWindowWithNameNonThreaded( char *Name );

#endif