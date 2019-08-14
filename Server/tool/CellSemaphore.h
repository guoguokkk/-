#ifndef CELL_SEMAPHORE_H_
#define CELL_SEMAPHORE_H_

#include<chrono>
#include<thread>
#include<condition_variable>//条件变量

//信号量
class CellSemaphore
{
public:
	//阻塞当前线程，等待_wakeup加1
	void wait()
	{
		std::unique_lock<std::mutex> lock(_mutex);//加锁，线程安全
		if (--_wait < 0)
		{
			//阻塞等待
			_cv.wait(lock, [this]()->bool {
				return _wakeup > 0;
				});
			--_wakeup;                                           
		}
	}

	//唤醒一个等待的线程
	void wakeup()
	{
		std::lock_guard<std::mutex> lock(_mutex);//加锁，线程安全
		if (++_wait <= 0)
		{
			++_wakeup;
			_cv.notify_one();//随机唤醒一个等待的线程
		}
	}

private:
	std::mutex _mutex;//改变数据缓冲区时需要加锁
	std::condition_variable _cv;//阻塞等待-条件变量
	int _wait = 0;//等待计数
	int _wakeup = 0;//唤醒计数
};

#endif // !CELL_SEMAPHORE_H_
