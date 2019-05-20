#ifndef CELL_TASK_H_
#define CELL_TASK_H_

#include<mutex>
#include<thread>
#include<list>//��vector�ʺϿ�������ɾ��
#include<functional>

//ִ������ķ�����
class CellTaskServer
{
	typedef std::function<void()> CellTask;

protected:
	//��������	
	virtual void onRun()
	{
		while (_isRun)
		{
			//�����񻺳���ȡ�����񣬷ŵ��������
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
				//���������Ҫ����
				while (!_tasks.empty())
				{
					auto pTask = _tasks.front();
					_tasks.pop_front();
					pTask();
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
		_isRun = true;
		std::thread t(std::mem_fn(&CellTaskServer::onRun), this);
		t.detach();
	}
	//�رչ����߳�
	virtual void closeTask()
	{
		_isRun = false;
	}

	//��������ӵ��������
	virtual void addTask(CellTask pTask)
	{
		std::lock_guard<std::mutex> lock(_mutex);//�Խ���
		_taskBuf.push_back(pTask);
	}

public:
	int _serverId = -1;//�����ķ�����id

private:
	std::list<CellTask> _tasks;//�������
	std::list<CellTask> _taskBuf;//���񻺳����������������߲��ݵ������񻺳���
	std::mutex _mutex;//���񻺳�������
	bool _isRun = false;
};
#endif // !CELL_TASK_H_

