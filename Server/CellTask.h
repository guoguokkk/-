#ifndef CELL_TASK_H_
#define CELL_TASK_H_

#include<mutex>
#include<thread>
#include<list>//比vector适合快速增加删除
#include<functional>
#include"CellSemaphore.h"
#include"CellThread.h"

//执行任务的服务类
class CellTaskServer
{
	typedef std::function<void()> CellTask;

protected:
	//工作函数	
	virtual void onRun(CellThread* pThread)
	{
		while (pThread->isRun())
		{
			//从任务缓冲区取出任务，放到任务队列
			if (!_taskBuf.empty())
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pTask : _taskBuf)
				{
					_tasks.push_back(pTask);
				}
				_taskBuf.clear();
			}

			//如果没有任务
			if (_tasks.empty())
			{
				std::chrono::milliseconds t(1);
				std::this_thread::sleep_for(t);
				continue;
			}

			//处理任务
			for (auto pTask : _tasks)
			{
				pTask();
			}

			//清空任务
			_tasks.clear();
		}
		printf("CellTaskServer%d.OnRun exit\n", serverId);		
		//_sem.wakeup();
	}
public:
	//启动工作线程
	virtual void startTask()	
	{
		_isRun = true;
		_thread.startThread(nullptr, [this](CellThread* pThread) {
			onRun(pThread);
			}, nullptr);
	}

	//关闭工作线程
	virtual void closeTask()
	{
		printf("CellTaskServer%d.Close begin\n", serverId);
		_thread.closeThread();
		printf("CellTaskServer%d.Close end\n", serverId);
	}

	//将任务添加到任务队列
	virtual void addTask(CellTask pTask)
	{
		std::lock_guard<std::mutex> lock(_mutex);//自解锁
		_taskBuf.push_back(pTask);
	}

public:
	int serverId = -1;//所属的服务器id

private:
	std::list<CellTask> _tasks;//任务队列
	std::list<CellTask> _taskBuf;//任务缓冲区，生产者消费者操纵的是任务缓冲区
	std::mutex _mutex;//任务缓冲区的锁
	bool _isRun = false;
	CellThread _thread;
};
#endif // !CELL_TASK_H_

