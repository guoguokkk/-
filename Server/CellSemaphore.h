#ifndef CELL_SEMAPHORE_H_
#define CELL_SEMAPHORE_H_

#include<chrono>
#include<thread>
#include<condition_variable>

//�ź���
class CellSemaphore
{
public:
	CellSemaphore()
	{
		_wait = 0;
		_wakeup = 0;
	}

	void wait()
	{
		std::unique_lock<std::mutex> lock(_mutex);
		if (--_wait < 0)
		{
			_cv.wait(lock, [this]()->bool {
				return _wakeup > 0;
				});//�����ȴ�OnRun()�˳�
			--_wakeup;
		}
	}

	void wakeup()
	{
		std::lock_guard<std::mutex> lock(_mutex);
		if (++_wait <= 0)
		{
			++_wakeup;
			_cv.notify_one();
		}
		else
		{
			printf("CELLSemaphore wakeup error.");
		}
	}

private:
	std::mutex _mutex;
	std::condition_variable _cv;//���������������ȴ�
	int _wait;//�ȴ�����
	int _wakeup;//���Ѽ���
};

#endif // !CELL_SEMAPHORE_H_
