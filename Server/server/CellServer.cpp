#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>
#include<vector>

CellServer::CellServer(int id) :_id(id)
{
	_pNetEvent = nullptr;
	_taskServer.cellServerId = id;
}

CellServer::~CellServer()
{
	CELLLOG_INFO("CellServer%d.~CellServer exit begin", _id);
	closeCellServer();
	CELLLOG_INFO("CellServer%d.~CellServer exit end", _id);
}

//绑定网络事件	
void CellServer::setEventObj(INetEvent* event)
{
	_pNetEvent = event;
}

//关闭消息处理业务
void CellServer::closeCellServer()
{
	CELLLOG_INFO("CellServer%d.Close begin", _id);
	_taskServer.closeTask();
	_thread.closeThread();
	CELLLOG_INFO("CellServer%d.Close end", _id);
}

//消息处理业务，参数为消息处理线程 pThread
void CellServer::onRunCellServer(CellThread* pThread)
{
	while (pThread->isRun())
	{
		//从缓冲队列里取出客户数据，加入正式客户队列
		if (!_clientsBuf.empty())
		{
			std::lock_guard<std::mutex> lock(_mutex);//自解锁
			for (auto pClient : _clientsBuf)
			{
				_clients[pClient->getSockfd()] = pClient;//std::map
				pClient->cellServerId = _id;

				if (_pNetEvent)
					_pNetEvent->onNetJoin(pClient);
			}
			_clientsBuf.clear();
			_clientsChange = true;
		}

		//如果正式客户队列没有需要处理的客户端，跳过
		if (_clients.empty())
		{
			CellThread::sleepInThread(1);//休眠
			_oldTime = CellTime::getNowInMillSec();//旧的时间戳更新
			continue;
		}

		//检测心跳消息，完成定时发送数据(没有定时发送了)
		checkTime();

		//计算可读集合、可写集合，并处理
		int ret = doSelect();
		if (ret == false)
		{
			pThread->exitThread();
			break;
		}

		//处理消息
		doMsg();
	}
	CELLLOG_INFO("CellServer%d.OnRun exit", _id);
}

//计算可读、可写集合，并处理
bool CellServer::doSelect()
{
	/////////////////////////////////////////////////////////
	//计算可读集合	
	if (_clientsChange)
	{
		//如果客户列表发生改变
		_clientsChange = false;
		_fdRead.zero();//清理集合

		//将所有的描述符都添加到可读集合，并找到最大的描述符
		_maxSock = _clients.begin()->second->getSockfd();
		for (auto client : _clients)
		{
			_fdRead.add(client.second->getSockfd());
			if (_maxSock < client.second->getSockfd())
			{
				_maxSock = client.second->getSockfd();
			}
		}

		//备份可读集合
		_fdReadBak.copy(_fdRead);
	}
	else
	{
		//可读集合没有改变，直接拷贝内容
		_fdRead.copy(_fdReadBak);
	}

	/////////////////////////////////////////////////////////
	//计算可写集合
	bool bNeedWrite = false;//是否需要写数据
	_fdWrite.zero();
	for (auto client : _clients)
	{
		//需要写数据的客户端,才加入fd_set检测是否可写
		if (client.second->needWrite())
		{
			bNeedWrite = true;
			_fdWrite.add(client.second->getSockfd());
		}
	}

	/////////////////////////////////////////////////////////
	//计算可写集合
	timeval time;
	time.tv_sec = 0;//秒
	time.tv_usec = 1;
	int ret = 0;
	if (bNeedWrite)
	{
		//返回发生可读事件的描述符和发生可写事件的描述符
		ret = select(_maxSock + 1, _fdRead.fdset(), _fdWrite.fdset(), nullptr, &time);
	}
	else
	{
		//返回发生可读事件的描述符
		ret = select(_maxSock + 1, _fdRead.fdset(), nullptr, nullptr, &time);
	}

	//select返回值：超时返回0;失败返回-1；成功返回大于0的整数，这个整数表示就绪描述符的数目
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

	//处理可读集合数据
	readData();

	//处理可写集合数据
	writeData();
	return true;
}

//处理可读集合数据-接收消息
void CellServer::readData()
{
#ifdef _WIN32
	auto pfdset = _fdRead.fdset();
	for (int i = 0; i < pfdset->fd_count; ++i)
	{
		auto iter = _clients.find(pfdset->fd_array[i]);
		if (iter != _clients.end())
		{
			int ret = recvData(iter->second);//触发<接收到网络数据>事件
			if (ret == SOCKET_ERROR)
			{
				//客户端离开
				onClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
	}

#else
	for (auto iter = _clients.begin(); iter != _clients.end(); )
	{
		if (_fdRead.has(iter->second->getSockfd()))
		{
			int ret = recvData(iter->second);//接收消息
			if (ret == SOCKET_ERROR)
			{
				//客户端离开
				onClientLeave(iter->second);
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

//处理可写集合数据-发送消息
void CellServer::writeData()
{
#ifdef _WIN32
	auto pfdset = _fdWrite.fdset();
	for (int i = 0; i < pfdset->fd_count; ++i)
	{
		auto iter = _clients.find(pfdset->fd_array[i]);
		if (iter != _clients.end())
		{
			int ret = iter->second->sendDataReal();//发送消息
			if (ret == SOCKET_ERROR)
			{
				onClientLeave(iter->second);
				_clients.erase(iter);
			}
		}
	}

#else
	for (auto iter = _clients.begin(); iter != _clients.end(); )
	{
		if (iter->second->needWrite() && _fdWrite.has(iter->second->getSockfd()))
		{
			int ret = iter->second->sendDataReal();//发送消息
			if (ret == -1)
			{
				onClientLeave(iter->second);
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

//检测心跳消息，完成定时发送数据 
void CellServer::checkTime()
{
	auto nowTime = CellTime::getNowInMillSec();//获取当前时间
	auto dt = nowTime - _oldTime;
	_oldTime = nowTime;

	for (auto iter = _clients.begin(); iter != _clients.end();)
	{
		//心跳检测
		if (iter->second->checkHeart(dt))
		{
			//心跳检测结果为死亡，移除对应的客户端
			if (_pNetEvent != nullptr)
				_pNetEvent->onNetLeave(iter->second);

			_clientsChange = true;
			auto iterOld = iter;
			++iter;
			_clients.erase(iterOld);
			continue;
		}

		////定时发送检测
		//iter->second->checkSend(dt);

		++iter;
	}
}

//清理正式客户队列和缓冲客户队列
void CellServer::clearClients()
{
	_clients.clear();
	_clientsBuf.clear();
}

//客户端离开
void CellServer::onClientLeave(std::shared_ptr<CellClient> pClient)
{
	if (_pNetEvent)
	{
		_pNetEvent->onNetLeave(pClient);
	}
	_clientsChange = true;
}

//触发<接收到网络数据>事件，参数为CellClient对象指针 pClient
int CellServer::recvData(std::shared_ptr<CellClient> pClient)
{
	int nLen = pClient->recvData();//接收客户端消息，直接使用每个客户端的消息缓冲区接收数据	
	if (nLen <= 0)
	{
		//客户端退出
		CELLLOG_INFO("Client %d exit.", (int)pClient->getSockfd());
		return -1;
	}
	_pNetEvent->onNetRecv(pClient);//触发<接收到网络数据>事件
	return nLen;
}

//处理消息
void CellServer::doMsg()
{
	std::shared_ptr<CellClient> pClient;
	for (auto client : _clients)
	{
		pClient = client.second;
		//循环 判断是否有消息需要处理
		while (pClient->hasMsg())
		{
			onNetMsg(pClient, pClient->front_msg());//处理第一条消息		
			pClient->pop_front_msg();//移除第一条消息
		}
	}
}

//处理网络消息
void CellServer::onNetMsg(std::shared_ptr<CellClient>& pClient, netmsg_DataHeader* header)
{
	_pNetEvent->onNetMsg(this, pClient, header);
}

//把新连接到的客户端加入当前CellServer的客户端缓冲队列，参数为CellClient对象 pClient
void CellServer::addClient(std::shared_ptr<CellClient> pClient)
{
	//加锁，自解锁
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

//启动任务执行和消息处理业务线程
void CellServer::startCellServer()
{
	//开启任务执行
	_taskServer.startTask();

	//开启消息处理业务线程
	_thread.startThread(nullptr,
		[this](CellThread* pThread) {
			onRunCellServer(pThread);
		},
		[this](CellThread* pThread) {
			clearClients();
		});
}

//获取客户端总数
size_t CellServer::getClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}

//void CellServer::addSendTask(std::shared_ptr<CellClient> pClient, netmsg_DataHeader* header)
//{
//	_taskServer.addTask([pClient,header]() {
//		pClient->sendData(header);
//		delete header;
//		});
//}
