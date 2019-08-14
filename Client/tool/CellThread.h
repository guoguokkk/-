#ifndef CELL_THREAD_H_
#define CELL_THREAD_H_

#include"../tool/CellSemaphore.h"
#include<functional>

//线程控制类
class CellThread
{
private:
	typedef std::function<void(CellThread*)> EventCall;//函数指针，返回值为void，参数为CellThread*

public:
	//当前线程休眠一段时间，参数为休眠时间dt
	static void sleepInThread(time_t dt)
	{
		std::chrono::milliseconds t(dt);
		std::this_thread::sleep_for(t);//sleep_for: 线程休眠某个指定的时间片(time span)，该线程才被重新唤醒
	}

	//启动线程，参数为创建事件，运行事件，销毁事件
	void startThread(EventCall onCreate = nullptr,
		EventCall onRun = nullptr,
		EventCall onDestroy = nullptr)
	{
		std::lock_guard<std::mutex> lg(_mutex);

		//避免重复启动线程
		if (!_isRun)
		{
			_isRun = true;

			if (onCreate)
				_onCreate = onCreate;
			if (onRun)
				_onRun = onRun;
			if (onDestroy)
				_onDestroy = onDestroy;

			//线程，std::mem_fn是std::mem_fun的增强版，不需要区分传递指针或者传递引用。而且可以支持传递多个参数。
			std::thread t(std::mem_fn(&CellThread::onWork), this);//创建线程，执行onWork函数，onWork函数的参数是this
			t.detach();
		}
	}

	//关闭线程
	void closeThread()
	{
		std::lock_guard<std::mutex> lg(_mutex);
		if (_isRun)
		{
			_isRun = false;
			_sem.wait();//阻塞线程，等待工作函数完成
		}
	}

	//退出线程
	void exitThread()
	{
		std::lock_guard<std::mutex> lg(_mutex);
		if (_isRun)
		{
			_isRun = false;//不需要使用信号量来阻塞等待，都则会一直阻塞在这里，因为不会被唤醒了
		}
	}

	//线程是否启动运行状态
	bool IsRun()
	{
		return _isRun;
	}
protected:
	//线程的运行时的工作函数
	void onWork()
	{
		if (_onCreate)
			_onCreate(this);
		if (_onRun)
			_onRun(this);
		if (_onDestroy)
			_onDestroy(this);

		_sem.wakeup();//唤醒关闭线程函数
		_isRun = false;
	}
private:
	EventCall _onCreate;//创建事件
	EventCall _onRun;//运行事件
	EventCall _onDestroy;//销毁事件

	CellSemaphore _sem;//信号量，控制线程的终止、退出
	std::mutex _mutex;//不同线程中改变数据时需要加锁	
	bool _isRun = false;//是否运行中
};

#endif // !CELL_THREAD_H_
