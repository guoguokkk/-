#ifndef TIME_STAMP_H
#define TIME_STAMP_H
#include<chrono>
using namespace std::chrono;
//�߾��ȼ�ʱ��
class TimeStamp {
public:
	TimeStamp() { Update(); }
	~TimeStamp() = default;

	//����ʱ��
	void Update()
	{
		_begin = high_resolution_clock::now();
	}

	//��ȡ��
	double GetElapsedSecond()
	{
		return GetElapsedTimeInMicroSec()* 0.000001;
	}

	//��ȡ����
	double GetElapsedTimeInMillSec()
	{
		return GetElapsedTimeInMicroSec()* 0.001;
	}
	//��ȡ΢��
	long long GetElapsedTimeInMicroSec()
	{
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}
private:
	time_point<high_resolution_clock> _begin;
};

#endif // !TIME_STAMP_H
