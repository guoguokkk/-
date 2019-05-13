#include "CellTask.h"

//��������	
void TaskServer::onRun()
{
	while (true)
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

void TaskServer::startTask()
{
	std::thread t(std::mem_fn(&TaskServer::onRun), this);
	t.detach();
}

void TaskServer::addTask(std::shared_ptr<CellTask> pTask)
{
	std::lock_guard<std::mutex> lock(_mutex);//�Խ���
	_taskBuf.push_back(pTask);
}

