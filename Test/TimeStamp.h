#ifndef TIME_STAMP_H
#define TIME_STAMP_H
#include<chrono>
using namespace std::chrono;
//高精度计时器
class TimeStamp {
public:
	TimeStamp() { Update(); }
	~TimeStamp() = default;

	//更新时间
	void Update()
	{
		_begin = high_resolution_clock::now();
	}

	//获取秒
	double GetElapsedSecond()
	{
		return GetElapsedTimeInMicroSec()* 0.000001;
	}

	//获取毫秒
	double GetElapsedTimeInMillSec()
	{
		return GetElapsedTimeInMicroSec()* 0.001;
	}
	//获取微秒
	long long GetElapsedTimeInMicroSec()
	{
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}
private:
	time_point<high_resolution_clock> _begin;
};

#endif // !TIME_STAMP_H
