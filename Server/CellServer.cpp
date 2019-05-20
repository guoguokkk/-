#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>
#include<vector>

CellServer::CellServer(int id)
{
	_id = id;
	_pNetEvent = nullptr;
	_isRun = false;
	_clientsChange = true;
	_taskServer._serverId = id;
}

CellServer::~CellServer()
{
	printf("CellServer%d.~CellServer exit begin\n", _id);
	closeServer();
	printf("CellServer%d.~CellServer exit end\n", _id);
}

void CellServer::setEventObj(INetEvent* event)
{
	_pNetEvent = event;
}

void CellServer::closeServer()
{
	printf("CellServer%d.Close begin\n", _id);

	//�����ظ��رգ�
	if (_isRun)
	{
		_taskServer.closeTask();
		_isRun = false;
		_sem.wait();
	}
	printf("CellServer%d.Close end\n", _id);
}

void CellServer::onRun()
{
	while (_isRun)
	{
		//�ӻ��������ȡ���ͻ����ݣ�������ʽ�ͻ����У��ͻ��˼�����Ҫ�ı�
		if (!_clientsBuf.empty())
		{
			std::lock_guard<std::mutex> lock(_mutex);//�Խ���
			for (auto pClient : _clientsBuf)
			{
				_clients[pClient->getSockfd()] = pClient;//std::map
				pClient->serverId = _id;

				if (_pNetEvent)
					_pNetEvent->onNetJoin(pClient);
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
			for (auto client : _clients)
			{
				FD_SET(client.second->getSockfd(), &fd_read);
				if (_maxSock < client.second->getSockfd())
				{
					_maxSock = client.second->getSockfd();
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
			printf("select task end\n");
			closeServer();
			return;
		}

		readData(fd_read);
		checkTime();//�������
	}
	printf("CellServer%d.OnRun exit\n", _id);

	clearClients();
	_sem.wakeup();
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
				if (_pNetEvent)
				{
					_pNetEvent->onNetLeave(iter->second);
				}

				_clientsChange = true;
				_clients.erase(iter);
			}
		}
		else
		{
			printf("error. if (iter != _clients.end())\n");
		}
	}

#else
	std::vector<std::shared_ptr<CellClient>> temp;//��¼Ҫɾ���Ŀͻ���
	for (auto iter : _clients)
	{
		if (FD_ISSET(iter.second.get()->getSock(), &fd_read))
		{
			int ret = recvData(iter.second);//������Ϣ
			if (ret == -1)
			{			
				if (_pEvent)
				{
					_pEvent->onNetLeave(iter.second);
				}
				_clientsChange = true;
				temp.push_back(iter.second);
			}
		}
	}
	for (auto pClient : temp)
	{
		_clients.erase(pClient->getSock());
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
		//�������
		if (iter->second->checkHeart(dt))
		{//���������Ϊ�������Ƴ���Ӧ�Ŀͻ���
			if (_pNetEvent != nullptr)
				_pNetEvent->onNetLeave(iter->second);

			_clientsChange = true;
			auto iterOld = iter;
			++iter;
			_clients.erase(iterOld);
			continue;
		}

		//��ʱ���ͼ��
		iter->second->checkSend(dt);
		++iter;
	}
}

void CellServer::clearClients()
{
	_clients.clear();
	_clientsBuf.clear();
}

//������Ϣ������ճ�����ٰ�
int CellServer::recvData(std::shared_ptr<CellClient> pClient)
{
	//���տͻ�����Ϣ��ֱ��ʹ��ÿ���ͻ��˵���Ϣ��������������
	char* recvBuf = pClient->getMsgBuf() + pClient->getLastPos();

	int nLen = (int)recv(pClient->getSockfd(), recvBuf, RECV_BUF_SIZE - pClient->getLastPos(), 0);
	_pNetEvent->onNetRecv(pClient);//����
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
	_pNetEvent->onNetMsg(this, pClient, header);
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
	if (_isRun == false)
	{
		_isRun = true;
		_thread = std::thread(std::mem_fn(&CellServer::onRun), this);
		_thread.detach();
		_taskServer.startTask();
	}
}

size_t CellServer::getClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}
