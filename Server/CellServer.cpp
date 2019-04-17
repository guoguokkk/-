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
	//避免重复关闭！
	if (_serverSock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	//关闭每个客户端！
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
		//从缓冲队列里取出客户数据，加入正式客户队列，客户端加入需要改变
		if (_clientsBuf.size() > 0)
		{
			std::lock_guard<std::mutex> lock(_mutex);//自解锁
			for (auto pClient : _clientsBuf)
			{
				_clients[pClient->getSock()] = pClient;//std::map
			}
			_clientsBuf.clear();
			_clientsChange = true;
		}

		//如果没有需要处理的客户端，跳过
		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);//1毫秒
			std::this_thread::sleep_for(t);//休眠
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
		{//如果没有改变，直接拷贝内容
			memcpy(&fd_read, &_fdReadBack, sizeof(fd_set));
		}

		timeval time;
		time.tv_sec = 0;//秒
		time.tv_usec = 0;
		int ret = select(_maxSock + 1, &fd_read, nullptr, nullptr, &time);//阻塞
		if (ret < 0)
		{
			printf("Server %d select error.\n", (int)_serverSock);
			closeServer();
			return false;
		}
		else if (ret == 0)
		{
			continue;//没有数据
		}
		
#ifdef _WIN32
		for (int i = 0; i < fd_read.fd_count; ++i)
		{
			auto iter = _clients.find(fd_read.fd_array[i]);
			if (iter != _clients.end())
			{
				int ret = recvData(iter->second);//接收消息
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
		std::vector<ClientSock*> temp;//记录要删除的客户端
		for (auto iter : _clients)
		{
			if (FD_ISSET(iter.second->getSock(), &fd_read))
			{
				int ret = recvData(iter.second);//接收消息
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

//接收消息，处理粘包、少包
int CellServer::recvData(ClientSock* pClient)
{
	//接收客户端消息
	int nLen = (int)recv(pClient->getSock(), _recvBuf, RECV_BUF_SIZE, 0);
	//判断客户端是否退出
	if (nLen <= 0)
	{
		printf("Client %d exit.\n", (int)pClient->getSock());
		return -1;
	}

	//接收到的数据放入对应客户端消息缓冲区
	memcpy(pClient->getMsgBuf() + pClient->getLastPos(), _recvBuf, nLen);
	pClient->setLastPos(pClient->getLastPos() + nLen);

	//处理粘包、少包问题
	while (pClient->getLastPos() >= sizeof(Header))
	{
		Header* header = (Header*)pClient->getMsgBuf();
		if (pClient->getLastPos() >= header->data_length)
		{
			//更新消息缓冲区
			int temp_pos = pClient->getLastPos() - header->data_length;
			onNetMsg(pClient, header);
			memcpy(pClient->getMsgBuf(), pClient->getMsgBuf() + header->data_length, temp_pos);
			pClient->setLastPos(temp_pos);
		}
		else
		{
			break;//消息缓冲区剩余数据不够一条完整消息
		}
	}
	return 0;
}

//响应网络数据
void CellServer::onNetMsg(ClientSock* pClient, Header* header)
{
	_pEvent->onNetMsg(pClient, header);
}

//消费者取出缓冲队列中的客户端
void CellServer::addClient(ClientSock* pClient)
{
	//加锁，自解锁
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

void CellServer::startCellServer()
{
	_thread = std::thread(std::mem_fn(&CellServer::onRun), this);//适配器 std::mem_fun
}

size_t CellServer::getClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}
