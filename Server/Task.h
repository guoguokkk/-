#ifndef TASK_H_
#define TASK_H_

#include<mutex>
#include<thread>
#include<list>//��vector�ʺϿ�������ɾ��
#include<functional>

//������࣬�������߳��첽�������
class Task
{
public:
	Task() {}
	virtual ~Task() {}
	virtual void doTask() {}//ִ������
};

//ִ������ķ�����
class TaskServer
{
protected:
	//ִ������
	virtual void onRun()
	{
		while (true)
		{
			//�ӻ�����ȡ�����ݣ��ŵ��������
			if (!_taskBuf.empty())
			{
				std::lock_guard<std::mutex> lock(_mutex);
				for (auto pTask : _taskBuf)
				{
					_tasks.push_back(pTask);
				}
				_taskBuf.clear();
			}

			//���������Ҫ����
			if (!_tasks.empty())
			{
				while (!_tasks.empty())
				{
					auto pTask = _tasks.front();
					_tasks.pop_front();
					pTask->doTask();
				}
			}
			else
			{
				std::chrono::milliseconds t(1);//1����
				std::this_thread::sleep_for(t);//sleep_for: �߳�����ĳ��ָ����ʱ��Ƭ(time span)�����̲߳ű����»���
				continue;
			}
		}

	}
public:
	//���������߳�
	virtual void startTask()
	{
		std::thread t(std::mem_fn(&TaskServer::onRun), this);
		t.detach();
	}

	//��������ӵ��������
	virtual void addTask(std::shared_ptr<Task> pTask)
	{
		std::lock_guard<std::mutex> lock(_mutex);//�Խ���
		_taskBuf.push_back(pTask);
	}
private:
	std::list<std::shared_ptr<Task>> _tasks;//�������
	std::list<std::shared_ptr<Task>> _taskBuf;//���񻺳����������������߲��ݵ������񻺳���
	std::mutex _mutex;//���񻺳�������

};
#endif // !TASK_H_

