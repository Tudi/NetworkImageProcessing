#ifndef _TOOLS_H_
#define _TOOLS_H_

#include <list>

void GetDesktopResolution(int& horizontal, int& vertical);
void BarbaricTCharToChar( TCHAR *In, char *Out, int MaxLen );
HWND FindWindowWithNameNonThreaded( char *Name );

class ProcessNameList
{
public:
	ProcessNameList()
	{
		Count = 0;
	}
	~ProcessNameList();
	int					Count;
	std::list<char*>	Names;
};

void GetProcessNameList( ProcessNameList * );

#endif