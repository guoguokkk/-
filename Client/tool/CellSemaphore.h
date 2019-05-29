#ifndef CELL_SEMAPHORE_H_
#define CELL_SEMAPHORE_H_

#include<chrono>
#include<thread>
#include<condition_variable>

//信号量
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
				});//阻塞等待OnRun()退出
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
	}

private:
	std::mutex _mutex;
	std::condition_variable _cv;//条件变量：阻塞等待
	int _wait;//等待计数
	int _wakeup;//唤醒计数
};

#endif // !CELL_SEMAPHORE_H_
