#ifndef CELL_TASK_H_
#define CELL_TASK_H_

#include<mutex>
#include<thread>
#include<list>//��vector�ʺϿ�������ɾ��
#include<functional>

//�������
class CellTask
{
public:
	CellTask() {}
	virtual ~CellTask() {}
	virtual void doTask() {}//ִ������
};

//ִ������ķ�����
class TaskServer
{
protected:
	virtual void onRun();//��������	
public:
	virtual void startTask();//���������߳�	
	virtual void addTask(std::shared_ptr<CellTask> pTask);//��������ӵ��������

private:
	std::list<std::shared_ptr<CellTask>> _tasks;//�������
	std::list<std::shared_ptr<CellTask>> _taskBuf;//���񻺳����������������߲��ݵ������񻺳���
	std::mutex _mutex;//���񻺳�������
};
#endif // !CELL_TASK_H_

