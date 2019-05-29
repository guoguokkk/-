#ifndef CELL_THREAD_H_
#define CELL_THREAD_H_

#include"CellSemaphore.h"
#include<functional>
#include<mutex>

//线程控制类
class CellThread
{
private:
	typedef std::function<void(CellThread*)> EventCall;
public:
	//启动线程
	void startThread(EventCall onCreate = nullptr,
		EventCall onRun = nullptr,
		EventCall onDestroy = nullptr)
	{
		std::lock_guard<std::mutex> lg(_mutex);

		if (!_isRun)
		{			
			_isRun = true;

			if (onCreate)
				_onCreate = onCreate;
			if (onRun)
				_onRun = onRun;
			if (onDestroy)
				_onDestroy = onDestroy;

			std::thread t(std::mem_fn(&CellThread::onWork), this);
			t.detach();
		}
	}

	void closeThread()
	{
		std::lock_guard<std::mutex> lg(_mutex);
		if (_isRun)
		{
			_isRun = false;
			_sem.wait();
		}
	}

	void exitThread()
	{
		std::lock_guard<std::mutex> lg(_mutex);
		if (_isRun)
		{
			_isRun = false;
		}
	}

	//线程是否启动运行状态
	bool isRun()
	{
		return _isRun;
	}
protected:
	//工作函数
	void onWork()
	{
		if (_onCreate)
			_onCreate(this);
		if (_onRun)
			_onRun(this);
		if (_onDestroy)
			_onDestroy(this);

		_sem.wakeup();
	}
private:
	CellSemaphore _sem;
	std::mutex _mutex;//改变数据需要加锁
	EventCall _onCreate;//事件
	EventCall _onRun;//事件
	EventCall _onDestroy;//事件
	bool _isRun = false;//是否运行中
};

#endif // !CELL_THREAD_H_

