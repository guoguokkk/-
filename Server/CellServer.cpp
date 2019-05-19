#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>
#include<vector>

CellServer::CellServer(SOCKET serverSock)
{
	_serverSock = serverSock;
	_pEvent = nullptr;
}

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
		closesocket(iter.second->getSockfd());
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
		if (!_clientsBuf.empty())
		{
			std::lock_guard<std::mutex> lock(_mutex);//�Խ���
			for (auto pClient : _clientsBuf)
			{
				_clients[pClient->getSockfd()] = pClient;//std::map
			}
			_clientsBuf.clear();
			_clientsChange = true;
		}

		//���û����Ҫ����Ŀͻ��ˣ�����
		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);//1����
			std::this_thread::sleep_for(t);//����

			_oldTime = CellTime::getNowInMillSec();//�ɵ�ʱ�������

			continue;
		}

		fd_set fd_read;
		FD_ZERO(&fd_read);
		if (_clientsChange)
		{
			_clientsChange = false;
			_maxSock = _clients.begin()->second->getSockfd();
			for (auto iter : _clients)
			{
				FD_SET(iter.second->getSockfd(), &fd_read);
				if (_maxSock < iter.second->getSockfd())
				{
					_maxSock = iter.second->getSockfd();
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
		time.tv_usec = 1;
		int ret = select(_maxSock + 1, &fd_read, nullptr, nullptr, &time);//����
		if (ret < 0)
		{
			printf("Server %d select error.\n", (int)_serverSock);
			closeServer();
			return false;
		}

		readData(fd_read);
		checkTime();//�������
	}
}

void CellServer::readData(fd_set& fd_read)
{

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
				closesocket(iter->first);//�ͷſͻ���socket��Դ
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
				_clientsChange = true;
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

//�������
void CellServer::checkTime()
{
	auto nowTime = CellTime::getNowInMillSec();//��ȡ��ǰʱ��
	auto dt = nowTime - _oldTime;
	_oldTime = nowTime;
	for (auto iter = _clients.begin(); iter != _clients.end();)
	{
		if (iter->second->checkHeart(dt))
		{//���������Ϊ�������Ƴ���Ӧ�Ŀͻ���
			if (_pEvent != nullptr)
				_pEvent->onNetLeave(iter->second);

			_clientsChange = true;
			auto iterOld = iter;
			++iter;
			_clients.erase(iterOld);
		}
		else
		{
			++iter;
		}		
	}
}

bool CellServer::isRun()
{
	return _serverSock != INVALID_SOCKET;
}

//������Ϣ������ճ�����ٰ�
int CellServer::recvData(std::shared_ptr<CellClient> pClient)
{
	//���տͻ�����Ϣ��ֱ��ʹ��ÿ���ͻ��˵���Ϣ��������������
	char* recvBuf = pClient->getMsgBuf() + pClient->getLastPos();

	int nLen = (int)recv(pClient->getSockfd(), recvBuf, RECV_BUF_SIZE - pClient->getLastPos(), 0);
	_pEvent->onNetRecv(pClient);//����
	//�жϿͻ����Ƿ��˳�
	if (nLen <= 0)
	{
		printf("Client %d exit.\n", (int)pClient->getSockfd());
		return -1;
	}

	pClient->setLastPos(pClient->getLastPos() + nLen);

	//����ճ�����ٰ�����
	while (pClient->getLastPos() >= sizeof(netmsg_Header))
	{
		netmsg_Header* header = (netmsg_Header*)pClient->getMsgBuf();
		if (pClient->getLastPos() >= header->dataLength)
		{
			//������Ϣ������
			int temp_pos = pClient->getLastPos() - header->dataLength;
			onNetMsg(pClient, header);
			memcpy(pClient->getMsgBuf(), pClient->getMsgBuf() + header->dataLength, temp_pos);
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
void CellServer::onNetMsg(std::shared_ptr<CellClient> & pClient, netmsg_Header * header)
{
	_pEvent->onNetMsg(this, pClient, header);
}

//������ȡ����������еĿͻ���
void CellServer::addClient(std::shared_ptr<CellClient> pClient)
{
	//�������Խ���
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

void CellServer::startCellServer()
{
	_thread = std::thread(std::mem_fn(&CellServer::onRun), this);
	_taskServer.startTask();
}

size_t CellServer::getClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}


