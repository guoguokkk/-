#ifndef TIME_STAMP_H_
#define TIME_STAMP_H_
#include<chrono>
using namespace std::chrono;

class CellTime
{
public:
	//获取当前时间，毫秒
	static time_t getNowInMillSec()
	{
		return duration_cast<milliseconds>(high_resolution_clock::now().time_since_epoch()).count();
	}
};

//高精度计时器
class CellTimeStamp
{
public:
	CellTimeStamp()
	{
		update();
	}
	~CellTimeStamp() = default;

	//更新时间
	void update()
	{
		_begin = high_resolution_clock::now();
	}

	//获取秒
	double getElapsedSecond()
	{
		return getElapsedTimeInMicroSec()* 0.000001;
	}

	//获取毫秒
	double getElapsedTimeInMillSec()
	{
		return getElapsedTimeInMicroSec()* 0.001;
	}
	//获取微秒
	long long getElapsedTimeInMicroSec()
	{
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}
private:
	time_point<high_resolution_clock> _begin;
};

#endif // !TIME_STAMP_H_
