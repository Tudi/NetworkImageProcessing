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


struct SampleStore
{
	unsigned int Value;
	unsigned int Stamp;
};

class ShortTermDataUsageSampler
{
public:
	ShortTermDataUsageSampler()
	{
		HistoryDuration = 0;
	}
	int IsStarted( )
	{
		return ( HistoryDuration > 0 );
	}
	void Init( int HistoryDurationMilliSeconds )
	{
		HistoryDuration = HistoryDurationMilliSeconds;
		Samples.clear();
	}
	void			AddSample( unsigned int val );
	unsigned int	GetSumInterval();
private:
	std::list<SampleStore*> Samples;
	unsigned int			HistoryDuration;
};

char *stristr( char *SearchIn, char *SearchWhat );

#endif