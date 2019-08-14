#ifndef CELL_SERVERSELECT_H_
#define CELL_SERVERSELECT_H_
#include"../server/CellServer.h"

class CellServerSelect :public CellServer {
public:
	~CellServerSelect()
	{
		CloseCellServer();
	}

	//����ɶ�����д���ϣ�������
	bool DoNetEvents()
	{
		/////////////////////////////////////////////////////////
		//�ɶ�����	
		if (_clientsChange)
		{
			//����ͻ��б����ı�
			_clientsChange = false;
			_fdRead.Zero();//������

			//�����е�����������ӵ��ɶ����ϣ����ҵ�����������
			_maxSock = _clients.begin()->second->GetSockfd();
			for (auto client : _clients)
			{
				_fdRead.Add(client.second->GetSockfd());
				if (_maxSock < client.second->GetSockfd())
				{
					_maxSock = client.second->GetSockfd();
				}
			}

			//���ݿɶ�����
			_fdReadBak.Copy(_fdRead);
		}
		else
		{
			//�ɶ�����û�иı䣬ֱ�ӿ�������
			_fdRead.Copy(_fdReadBak);
		}

		/////////////////////////////////////////////////////////
		//��д����
		bool bNeedWrite = false;//�Ƿ���Ҫд����
		_fdWrite.Zero();
		for (auto client : _clients)
		{
			//��Ҫ��ͻ��˷�������,�ż���fd_set����Ƿ��д
			if (client.second->NeedWrite())
			{
				bNeedWrite = true;
				_fdWrite.Add(client.second->GetSockfd());
			}
		}

		/////////////////////////////////////////////////////////
		//����ɶ�����д����
		timeval time;
		time.tv_sec = 0;//��
		time.tv_usec = 1;
		int ret = 0;
		if (bNeedWrite)
		{
			//���ط����ɶ��¼����������ͷ�����д�¼���������
			ret = select(_maxSock + 1, _fdRead.GetFdSet(), _fdWrite.GetFdSet(), nullptr, &time);
		}
		else
		{
			//���ط����ɶ��¼���������
			ret = select(_maxSock + 1, _fdRead.GetFdSet(), nullptr, nullptr, &time);
		}

		//select����ֵ����ʱ����0;ʧ�ܷ���-1���ɹ����ش���0�����������������ʾ��������������Ŀ
		if (ret < 0)
		{
			CELLLOG_INFO("CELLServer%d.OnRun.select Error exit:errno<%d>,errmsg<%s>",
				_id, errno, strerror(errno));
			return false;
		}
		else if (ret == 0)
		{
			return true;
		}

		//����ɶ���������-������Ϣ
		ReadData();

		//�����д��������-������Ϣ
		WriteData();
		return true;
	}


	//����ɶ���������-������Ϣ
	void ReadData()
	{
#ifdef _WIN32
		auto pfdset = _fdRead.GetFdSet();

		//windows�¼�¼������������Ĵ�С�����������飬��������
		for (int i = 0; i < pfdset->fd_count; ++i)
		{
			auto iter = _clients.find(pfdset->fd_array[i]);
			if (iter != _clients.end())
			{
				int ret = RecvData(iter->second);//�ҵ��ˣ���������
				if (ret == SOCKET_ERROR)
				{
					//�ͻ����뿪
					OnClientLeave(iter->second);
					_clients.erase(iter);
				}
			}
		}

#else
		for (auto iter = _clients.begin(); iter != _clients.end(); )
		{
			if (_fdRead.Has(iter->second->getSockfd()))
			{
				int ret = RecvData(iter->second);//������Ϣ
				if (ret == SOCKET_ERROR)
				{
					//�ͻ����뿪
					OnClientLeave(iter->second);
					auto iterOld = iter;
					++iter;
					_clients.erase(iterOld);
					continue;
				}
			}
			++iter;
		}
#endif // _WIN32		
	}

	//�����д��������-������Ϣ
	void WriteData()
	{
#ifdef _WIN32
		auto pfdset = _fdWrite.GetFdSet();

		//windows�¼�¼������������Ĵ�С�����������飬��������
		for (int i = 0; i < pfdset->fd_count; ++i)
		{
			auto iter = _clients.find(pfdset->fd_array[i]);//����ʽ�ͻ��˶�����Ҫ��ĳ��������д�¼��Ŀͻ���
			if (iter != _clients.end())
			{
				int ret = iter->second->SendDataReal();//�ҵ��ˣ���ֱ�ӷ�����Ϣ
				if (ret == SOCKET_ERROR)
				{
					OnClientLeave(iter->second);
					_clients.erase(iter);
				}
			}
		}

#else
		for (auto iter = _clients.begin(); iter != _clients.end(); )
		{
			if (iter->second->NeedWrite() && _fdWrite.Has(iter->second->getSockfd()))
			{
				int ret = iter->second->SendDataReal();//������Ϣ
				if (ret == -1)
				{
					OnClientLeave(iter->second);
					auto iterOld = iter;
					++iter;
					_clients.erase(iter);
					continue;
				}
			}
			++iter;
		}
#endif // _WIN32	
	}
private:
	CellFDSet _fdRead;
	CellFDSet _fdWrite;
	CellFDSet _fdReadBak;//�ͻ��б���

	SOCKET _maxSock;
};

#endif // !CELL_SERVERSELECT_H_
