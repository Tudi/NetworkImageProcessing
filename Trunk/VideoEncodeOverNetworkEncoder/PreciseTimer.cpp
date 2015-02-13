#include "StdAfx.h"

double PCFreq = 0.0;
__int64 CounterStart = 0;

void StartTimer()
{
    LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li))
		printf( "QueryPerformanceFrequency failed!\n" );

    PCFreq = double(li.QuadPart)/1000.0;

    QueryPerformanceCounter(&li);
    CounterStart = li.QuadPart;
}
/*
double GetTimer()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return double(li.QuadPart-CounterStart)/PCFreq;
} */

unsigned int GetTimer()
{
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    return (unsigned int)( (li.QuadPart-CounterStart)/PCFreq );
}
