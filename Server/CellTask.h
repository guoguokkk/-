#ifndef CELL_TASK_H_
#define CELL_TASK_H_

#include<mutex>
#include<thread>
#include<list>//比vector适合快速增加删除
#include<functional>

//执行任务的服务类
class CellTaskServer
{
	typedef std::function<void()> CellTask;

protected:
	//工作函数	
	virtual void onRun()
	{
		while (_isRun)
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
					pTask();
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
public:
	//启动工作线程
	virtual void startTask()	
	{
		_isRun = true;
		std::thread t(std::mem_fn(&CellTaskServer::onRun), this);
		t.detach();
	}
	//关闭工作线程
	virtual void closeTask()
	{
		_isRun = false;
	}

	//将任务添加到任务队列
	virtual void addTask(CellTask pTask)
	{
		std::lock_guard<std::mutex> lock(_mutex);//自解锁
		_taskBuf.push_back(pTask);
	}

public:
	int _serverId = -1;//所属的服务器id

private:
	std::list<CellTask> _tasks;//任务队列
	std::list<CellTask> _taskBuf;//任务缓冲区，生产者消费者操纵的是任务缓冲区
	std::mutex _mutex;//任务缓冲区的锁
	bool _isRun = false;
};
#endif // !CELL_TASK_H_

