#ifndef CELL_TASK_H_
#define CELL_TASK_H_

#include<mutex>
#include<thread>
#include<list>//比vector适合快速增加删除
#include<functional>
#include"CellThread.h"

//执行任务的服务类
class CellTaskServer
{
private:
	typedef std::function<void()> CellTask;//函数指针，返回值为void

public:
	//将任务添加到任务缓冲队列
	virtual void addTask(CellTask task)
	{
		std::lock_guard<std::mutex> lock(_mutex);//对缓冲队列加锁
		_taskBuf.push_back(task);
	}

	//启动工作线程，一个任务对应一个线程
	virtual void startTask()
	{
		_thread.startThread(
			nullptr,
			[this](CellThread* pThread) {
			onRun(pThread);
			}, 
			nullptr);
	}

	//关闭工作线程
	virtual void closeTask()
	{
		_thread.closeThread();
	}

protected:
	//工作函数	
	void onRun(CellThread* pThread)
	{
		//工作线程还在运行
		while (pThread->IsRun())
		{
			//从任务缓冲队列取出任务，放到任务队列
			if (!_taskBuf.empty())
			{
				std::lock_guard<std::mutex> lock(_mutex);//对缓冲队列加锁，因为要清空缓冲队列
				for (auto pTask : _taskBuf)
				{
					_tasks.push_back(pTask);
				}
				_taskBuf.clear();
			}

			//如果没有任务
			if (_tasks.empty())
			{
				CellThread::sleepInThread(1);//线程休眠，让出CPU
				continue;
			}

			//依次处理任务队列中的任务
			for (auto pTask : _tasks)
			{
				pTask();
			}

			//清空任务队列
			_tasks.clear();
		}

		//当工作线程退出以后，需要依次处理任务缓冲队列中剩余的任务
		for (auto pTask : _taskBuf)
		{
			pTask();
		}
	}
	
public:
	int cellServerId = -1;//所属的服务器id，测试用

private:
	std::list<CellTask> _tasks;//任务队列
	std::list<CellTask> _taskBuf;//任务缓冲队列，生产者消费者操纵的是任务缓冲队列
	std::mutex _mutex;//任务缓冲队列的锁
	CellThread _thread;
};
#endif // !CELL_TASK_H_

