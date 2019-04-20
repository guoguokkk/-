#ifndef TASK_H_
#define TASK_H_

#include<mutex>
#include<thread>
#include<list>//比vector适合快速增加删除
#include<functional>

//任务基类，在其它线程异步完成任务
class Task
{
public:
	Task() {}
	virtual ~Task() {}
	virtual void doTask() {}//执行任务
};

//执行任务的服务类
class TaskServer
{
protected:
	//执行任务
	virtual void onRun()
	{
		while (true)
		{
			//从缓冲区取出数据，放到任务队列
			if (!_taskBuf.empty())
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pTask : _taskBuf)
				{
					_tasks.push_back(pTask);
				}
				_taskBuf.clear();
			}

			//如果有任务要处理
			if (!_tasks.empty())
			{
				while (!_tasks.empty())
				{
					Task* pTask = _tasks.front();
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
public:
	//启动工作线程
	virtual void startTask()
	{
		std::thread t(std::mem_fn(&TaskServer::onRun), this);
		t.detach();
	}

	//将任务添加到任务队列
	virtual void addTask(Task* pTask)
	{
		std::lock_guard<std::mutex> lock(_mutex);//自解锁
		_taskBuf.push_back(pTask);
	}
private:
	std::list<Task*> _tasks;//任务队列
	std::list<Task*> _taskBuf;//任务缓冲区，生产者消费者操纵的是任务缓冲区
	std::mutex _mutex;//任务缓冲区的锁

};
#endif // !TASK_H_

