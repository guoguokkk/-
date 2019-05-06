#include"MemoryAlloc.h"
#include"../Test/TimeStamp.h"
#include <stdlib.h>
#include<thread>
#include<iostream>
#include<mutex>

std::mutex m;
const int tCount = 16;
const int mCount = 1000000;
const int nCount = mCount / tCount;
void workFun(int index)
{
	char* data[nCount];
	for (size_t i = 0; i < nCount; i++)
	{
		data[i] = new char[(rand() % 128) + 1];
	}
	for (size_t i = 0; i < nCount; i++)
	{
		delete[] data[i];
	}
}

int main()
{
	std::thread t[tCount];
	for (int n = 0; n < tCount; n++)
	{
		t[n] = std::thread(workFun, n);
	}
	TimeStamp tTime;
	for (int n = 0; n < tCount; n++)
	{
		t[n].join();
	}
	std::cout << tTime.getElapsedTimeInMillSec() << std::endl;
	std::cout << "Hello,main thread." << std::endl;
	return 0;
}