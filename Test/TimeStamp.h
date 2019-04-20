#ifndef TIME_STAMP_H_
#define TIME_STAMP_H_
#include<chrono>
using namespace std::chrono;
//�߾��ȼ�ʱ��
class TimeStamp {
public:
	TimeStamp() 
	{
		update();
	}
	~TimeStamp() = default;

	//����ʱ��
	void update()
	{
		_begin = high_resolution_clock::now();
	}

	//��ȡ��
	double getElapsedSecond()
	{
		return getElapsedTimeInMicroSec()* 0.000001;
	}

	//��ȡ����
	double getElapsedTimeInMillSec()
	{
		return getElapsedTimeInMicroSec()* 0.001;
	}
	//��ȡ΢��
	long long getElapsedTimeInMicroSec()
	{
		return duration_cast<microseconds>(high_resolution_clock::now() - _begin).count();
	}
private:
	time_point<high_resolution_clock> _begin;
};

#endif // !TIME_STAMP_H_
