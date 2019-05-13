#include "CellTask.h"

//工作函数	
void TaskServer::onRun()
{
	while (true)
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
		
		if (_tasks.empty())
		{
			//如果有任务要处理
			while (!_tasks.empty())
			{
				auto pTask = _tasks.front();
				_tasks.pop_front();
				pTask->doTask();
			}
		}
		else
		{
			std::chrono::milliseconds t(1);//1毫秒
			std::this_thread::sleep_for(t);//sleep_for: 线程休眠某个指定的时间片(time span)，该线程才被重新唤醒
			continue;
		}
	}
}

void TaskServer::startTask()
{
	std::thread t(std::mem_fn(&TaskServer::onRun), this);
	t.detach();
}

void TaskServer::addTask(std::shared_ptr<CellTask> pTask)
{
	std::lock_guard<std::mutex> lock(_mutex);//自解锁
	_taskBuf.push_back(pTask);
}

