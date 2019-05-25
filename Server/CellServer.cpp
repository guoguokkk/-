#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>
#include<vector>

CellServer::CellServer(int id)
{
	_id = id;
	_pNetEvent = nullptr;
	_clientsChange = true;
	//_taskServer._serverId = id;
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
	_taskServer.closeTask();
	_thread.closeThread();
	printf("CellServer%d.Close end\n", _id);
}

void CellServer::onRunCellServer(CellThread* pThread)
{
	while (pThread->isRun())
	{
		//从缓冲队列里取出客户数据，加入正式客户队列，客户端加入需要改变
		if (!_clientsBuf.empty())
		{
			std::lock_guard<std::mutex> lock(_mutex);//自解锁
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

		//如果没有需要处理的客户端，跳过
		if (_clients.empty())
		{
			std::chrono::milliseconds t(1);//1毫秒
			std::this_thread::sleep_for(t);//休眠

			_oldTime = CellTime::getNowInMillSec();//旧的时间戳更新

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
		{//如果没有改变，直接拷贝内容
			memcpy(&fd_read, &_fdReadBack, sizeof(fd_set));
		}

		timeval time;
		time.tv_sec = 0;//秒
		time.tv_usec = 1;
		int ret = select(_maxSock + 1, &fd_read, nullptr, nullptr, &time);//阻塞
		if (ret < 0)
		{
			printf("select task end\n");
			pThread->exitThread();
			break;
		}

		readData(fd_read);
		checkTime();//检测心跳
	}
	printf("CellServer%d.OnRun exit\n", _id);
}

void CellServer::readData(fd_set& fd_read)
{

#ifdef _WIN32
	for (int i = 0; i < fd_read.fd_count; ++i)
	{
		auto iter = _clients.find(fd_read.fd_array[i]);
		if (iter != _clients.end())
		{
			int ret = recvData(iter->second);//接收消息
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
	std::vector<std::shared_ptr<CellClient>> temp;//记录要删除的客户端
	for (auto iter : _clients)
	{
		if (FD_ISSET(iter.second.get()->getSock(), &fd_read))
		{
			int ret = recvData(iter.second);//接收消息
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

//检测心跳
void CellServer::checkTime()
{
	auto nowTime = CellTime::getNowInMillSec();//获取当前时间
	auto dt = nowTime - _oldTime;
	_oldTime = nowTime;

	for (auto iter = _clients.begin(); iter != _clients.end();)
	{
		//心跳检测
		if (iter->second->checkHeart(dt))
		{//心跳检测结果为死亡，移除对应的客户端
			if (_pNetEvent != nullptr)
				_pNetEvent->onNetLeave(iter->second);

			_clientsChange = true;
			auto iterOld = iter;
			++iter;
			_clients.erase(iterOld);
			continue;
		}

		//定时发送检测
		iter->second->checkSend(dt);
		++iter;
	}
}

void CellServer::clearClients()
{
	_clients.clear();
	_clientsBuf.clear();
}

//接收消息，处理粘包、少包
int CellServer::recvData(std::shared_ptr<CellClient> pClient)
{
	//接收客户端消息，直接使用每个客户端的消息缓冲区接收数据
	char* recvBuf = pClient->getMsgBuf() + pClient->getLastPos();

	int nLen = (int)recv(pClient->getSockfd(), recvBuf, RECV_BUF_SIZE - pClient->getLastPos(), 0);
	_pNetEvent->onNetRecv(pClient);//计数
	//判断客户端是否退出
	if (nLen <= 0)
	{
		printf("Client %d exit.\n", (int)pClient->getSockfd());
		return -1;
	}

	pClient->setLastPos(pClient->getLastPos() + nLen);

	//处理粘包、少包问题
	while (pClient->getLastPos() >= sizeof(netmsg_Header))
	{
		netmsg_Header* header = (netmsg_Header*)pClient->getMsgBuf();
		if (pClient->getLastPos() >= header->dataLength)
		{
			//更新消息缓冲区
			int temp_pos = pClient->getLastPos() - header->dataLength;
			onNetMsg(pClient, header);
			memcpy(pClient->getMsgBuf(), pClient->getMsgBuf() + header->dataLength, temp_pos);
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
void CellServer::onNetMsg(std::shared_ptr<CellClient> & pClient, netmsg_Header * header)
{
	_pNetEvent->onNetMsg(this, pClient, header);
}

//消费者取出缓冲队列中的客户端
void CellServer::addClient(std::shared_ptr<CellClient> pClient)
{
	//加锁，自解锁
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

void CellServer::startCellServer()
{
	_taskServer.startTask();
	_thread.startThread(nullptr,
		[this](CellThread * pThread) {
			onRunCellServer(pThread);
		},
		[this](CellThread * pThread) {
			clearClients();
		});
}

size_t CellServer::getClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}

void CellServer::addSendTask(std::shared_ptr<CellClient> pClient, std::shared_ptr<netmsg_Header> header)
{
	auto task = std::make_shared<SendMsgToClientTask>(pClient, header);

	//执行任务
	_taskServer.addTask([pClient, header]() {
		pClient->sendData(header);
		});
}