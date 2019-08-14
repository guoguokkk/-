#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>
#include<vector>

CellServer::~CellServer()
{
	CELLLOG_INFO("CellServer%d.~CellServer exit begin", _id);
	CloseCellServer();
	CELLLOG_INFO("CellServer%d.~CellServer exit end", _id);
}

//设置id
void CellServer::SetId(int id)
{
	_id = id;
	_taskServer.cellServerId = id;
}

//绑定网络事件	
void CellServer::SetEventObj(INetEvent* event)
{
	_pNetEvent = event;
}

//关闭消息处理业务
void CellServer::CloseCellServer()
{
	CELLLOG_INFO("CellServer%d.Close begin", _id);
	_taskServer.closeTask();
	_thread.closeThread();
	CELLLOG_INFO("CellServer%d.Close end", _id);
}

//消息处理业务，参数为消息处理线程 pThread
void CellServer::OnRunCellServer(CellThread* pThread)
{
	while (pThread->IsRun())
	{
		//从缓冲队列里取出客户数据，加入正式客户队列
		if (!_clientsBuf.empty())
		{
			std::lock_guard<std::mutex> lock(_mutex);//自解锁
			for (auto pClient : _clientsBuf)
			{
				_clients[pClient->GetSockfd()] = pClient;//std::map
				pClient->cellServerId = _id;

				if (_pNetEvent)
					_pNetEvent->onNetJoin(pClient);

				OnClientJoin(pClient);
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
		CheckTime();

		//计算可读集合、可写集合，并处理
		int ret = DoNetEvents();
		if (ret == false)
		{
			pThread->exitThread();
			break;
		}

		//处理消息
		DoMsg();
	}
	CELLLOG_INFO("CellServer%d.OnRun exit", _id);
}

//检测心跳消息，完成定时发送数据 
void CellServer::CheckTime()
{
	auto nowTime = CellTime::getNowInMillSec();//获取当前时间
	auto dt = nowTime - _oldTime;
	_oldTime = nowTime;

	for (auto iter = _clients.begin(); iter != _clients.end();)
	{
		//心跳检测
		if (iter->second->CheckHeart(dt))
		{
			//心跳检测结果为死亡，移除对应的客户端
			if (_pNetEvent != nullptr)
				_pNetEvent->onNetLeave(iter->second);

			_clientsChange = true;
			delete iter->second;
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
void CellServer::ClearClients()
{
	for (auto client : _clients)
		delete client.second;
	_clients.clear();

	for (auto client : _clientsBuf)
		delete client;
	_clientsBuf.clear();
}

//客户端离开
void CellServer::OnClientLeave(CellClient* pClient)
{
	if (_pNetEvent)
	{
		_pNetEvent->onNetLeave(pClient);
	}
	_clientsChange = true;
	delete pClient;
}

void CellServer::OnClientJoin(CellClient* pClient)
{
}

//触发<接收到网络数据>事件，参数为CellClient对象指针 pClient
int CellServer::RecvData(CellClient* pClient)
{
	int nLen = pClient->RecvData();//接收客户端消息，直接使用每个客户端的消息缓冲区接收数据	
	if (nLen <= 0)
	{
		//客户端退出
		CELLLOG_INFO("Client %d exit.", (int)pClient->GetSockfd());
		return -1;
	}
	_pNetEvent->onNetRecv(pClient);//触发<接收到网络数据>事件
	return nLen;
}

//处理消息
void CellServer::DoMsg()
{
	CellClient* pClient;
	for (auto client : _clients)
	{
		pClient = client.second;
		//循环 判断是否有消息需要处理
		while (pClient->HasMsg())
		{
			onNetMsg(pClient, pClient->GetFrontMsg());//处理第一条消息		
			pClient->PopFrontMsg();//移除第一条消息
		}
	}
}

//处理网络消息
void CellServer::onNetMsg(CellClient* pClient, netmsg_DataHeader* header)
{
	_pNetEvent->onNetMsg(this, pClient, header);
}

//把新连接到的客户端加入当前CellServer的客户端缓冲队列，参数为CellClient对象 pClient
void CellServer::AddClient(CellClient* pClient)
{
	//加锁，自解锁
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

//启动任务执行和消息处理业务线程
void CellServer::StartCellServer()
{
	//开启任务执行
	_taskServer.startTask();

	//开启消息处理业务线程
	_thread.startThread(nullptr,
		[this](CellThread* pThread) {
			OnRunCellServer(pThread);
		},
		[this](CellThread* pThread) {
			ClearClients();
		});
}

//获取客户端总数
size_t CellServer::GetClientCount()
{
	return size_t(_clients.size() + _clientsBuf.size());
}

//void CellServer::addSendTask(CellClient* pClient, netmsg_DataHeader* header)
//{
//	_taskServer.addTask([pClient,header]() {
//		pClient->sendData(header);
//		delete header;
//		});
//}
