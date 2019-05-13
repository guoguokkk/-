#ifndef CELL_TASK_H_
#define CELL_TASK_H_

#include<mutex>
#include<thread>
#include<list>//比vector适合快速增加删除
#include<functional>

//任务基类
class CellTask
{
public:
	CellTask() {}
	virtual ~CellTask() {}
	virtual void doTask() {}//执行任务
};

//执行任务的服务类
class TaskServer
{
protected:
	virtual void onRun();//工作函数	
public:
	virtual void startTask();//启动工作线程	
	virtual void addTask(std::shared_ptr<CellTask> pTask);//将任务添加到任务队列

private:
	std::list<std::shared_ptr<CellTask>> _tasks;//任务队列
	std::list<std::shared_ptr<CellTask>> _taskBuf;//任务缓冲区，生产者消费者操纵的是任务缓冲区
	std::mutex _mutex;//任务缓冲区的锁
};
#endif // !CELL_TASK_H_

