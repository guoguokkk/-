#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>
#include<vector>

CellServer::~CellServer()
{
	closeServer();
	_serverSock = INVALID_SOCKET;
}

void CellServer::setEventObj(INetEvent* event)
{
	_pEvent = event;
}

void CellServer::closeServer()
{
	//�����ظ��رգ�
	if (_serverSock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	//�ر�ÿ���ͻ��ˣ�
	for (auto iter : _clients)
	{
		closesocket(iter.second->getSock());
		delete iter.second;
	}
	closesocket(_serverSock);
#else
	for (auto iter : _clients)
	{
		close(iter.second->getSock());
		delete iter.second;
	}
	close(_serverSock);
#endif // _WIN32
	_clients.clear();
}

bool CellServer::onRun()
{
	_clientsChange = true;
	while (isRun())
	{
		//�ӻ��������ȡ���ͻ����ݣ�������ʽ�ͻ����У��ͻ��˼�����Ҫ�ı�
		if (_clientsBuf.size() > 0)
		{
			std::lock_guard<std::mutex> lock(_mutex);//�Խ���
			for (auto pClient : _clientsBuf)
			{
				_clients[pClient->getSock()] = pClient;//std::map
			}
			_clientsBuf.clear();
			_clientsChange = true;
		}

		//���û����Ҫ����Ŀͻ��ˣ�����
		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);//1����
			std::this_thread::sleep_for(t);//����
			continue;
		}

		fd_set fd_read;
		FD_ZERO(&fd_read);
		if (_clientsChange)
		{
			_clientsChange = false;
			_maxSock = _clients.begin()->second->getSock();
			for (auto iter : _clients)
			{
				FD_SET(iter.second->getSock(), &fd_read);
				if (_maxSock < iter.second->getSock())
				{
					_maxSock = iter.second->getSock();
				}
			}
			memcpy(&_fdReadBack, &fd_read, sizeof(fd_set));
		}
		else
		{//���û�иı䣬ֱ�ӿ�������
			memcpy(&fd_read, &_fdReadBack, sizeof(fd_set));
		}

		timeval time;
		time.tv_sec = 0;//��
		time.tv_usec = 0;
		int ret = select(_maxSock + 1, &fd_read, nullptr, nullptr, &time);//����
		if (ret < 0)
		{
			printf("Server %d select error.\n", (int)_serverSock);
			closeServer();
			return false;
		}
		else if (ret == 0)
		{
			continue;//û������
		}
		
#ifdef _WIN32
		for (int i = 0; i < fd_read.fd_count; ++i)
		{
			auto iter = _clients.find(fd_read.fd_array[i]);
			if (iter != _clients.end())
			{
				int ret = recvData(iter->second);//������Ϣ
				if (ret == -1)
				{
					if (_pEvent)
					{
						_pEvent->onNetLeave(iter->second);
					}

					_clientsChange = true;
					_clients.erase(iter->first);
				}
			}			
			else
			{
				printf("error. if (iter != _clients.end())\n");
			}
		}

#else
		std::vector<ClientSock*> temp;//��¼Ҫɾ���Ŀͻ���
		for (auto iter : _clients)
		{
			if (FD_ISSET(iter.second->getSock(), &fd_read))
			{
				int ret = recvData(iter.second);//������Ϣ
				if (ret == -1)
				{
					_clientsChange = false;
					temp.push_back(iter.second);
					if (_pEvent)
					{
						_pEvent->onNetLeave(iter.second);
					}
				}
			}
		}
		for (auto pClient : temp)
		{
			_clients.erase(pClient->getSock());
			delete pClient;
		}
#endif // _WIN32		
	}
}

bool CellServer::isRun()
{
	return _serverSock != INVALID_SOCKET;
}

//������Ϣ������ճ�����ٰ�
int CellServer::recvData(ClientSock* pClient)
{
	//���տͻ�����Ϣ
	int nLen = (int)recv(pClient->getSock(), _recvBuf, RECV_BUF_SIZE, 0);
	//�жϿͻ����Ƿ��˳�
	if (nLen <= 0)
	{
		printf("Client %d exit.\n", (int)pClient->getSock());
		return -1;
	}

	//���յ������ݷ����Ӧ�ͻ�����Ϣ������
	memcpy(pClient->getMsgBuf() + pClient->getLastPos(), _recvBuf, nLen);
	pClient->setLastPos(pClient->getLastPos() + nLen);

	//����ճ�����ٰ�����
	while (pClient->getLastPos() >= sizeof(Header))
	{
		Header* header = (Header*)pClient->getMsgBuf();
		if (pClient->getLastPos() >= header->data_length)
		{
			//������Ϣ������
			int temp_pos = pClient->getLastPos() - header->data_length;
			onNetMsg(pClient, header);
			memcpy(pClient->getMsgBuf(), pClient->getMsgBuf() + header->data_length, temp_pos);
			pClient->setLastPos(temp_pos);
		}
		else
		{
			break;//��Ϣ������ʣ�����ݲ���һ��������Ϣ
		}
	}
	return 0;
}

//��Ӧ��������
void CellServer::onNetMsg(ClientSock* pClient, Header* header)
{
	_pEvent->onNetMsg(pClient, header);
}

//������ȡ����������еĿͻ���
void CellServer::addClient(ClientSock* pClient)
{
	//�������Խ���
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

void CellServer::startCellServer()
{
	_thread = std::thread(std::mem_fn(&CellServer::onRun), this);//������ std::mem_fun
}

size_t CellServer::getClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}
