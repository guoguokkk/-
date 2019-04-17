#include "CellServer.h"
#include<iostream>
#include<functional>
#include<chrono>

CellServer::CellServer(SOCKET server_sock = INVALID_SOCKET)
{
	_server_sock = server_sock;
	_pEvent = nullptr;
}

CellServer::~CellServer()
{
	CloseServer();
	_server_sock = INVALID_SOCKET;
}

void CellServer::CloseServer()
{
	//避免重复关闭！
	if (_server_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	//关闭每个客户端！
	for (int i = 0; i < _clients.size(); ++i)
	{
		closesocket(_clients[i]->GetSock());
		delete _clients[i];
	}
	closesocket(_server_sock);
#else
	for (int i = 0; i < _client_groups.size(); ++i)
	{
		close(_client_groups[i]->GetSock());
		delete _client_groups[i];
	}
	close(_server_sock);
#endif // _WIN32
	_clients.clear();
}

bool CellServer::OnRun()
{
	while (true)
	{
		if (_server_sock == INVALID_SOCKET)
		{
			return false;
		}

		//从缓冲队列里取出客户数据，加入正式客户队列
		if (_clientsBuf.size() > 0)
		{
			std::lock_guard<std::mutex> lock(_mutex);//自解锁
			for (auto pClient : _clientsBuf)
			{
				_clients.push_back(pClient);
			}
			_clientsBuf.clear();
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

		SOCKET max_sock = _clients[0]->GetSock();
		for (int i = 0; i < _clients.size(); ++i)
		{
			FD_SET(_clients[i]->GetSock(), &fd_read);
			if (max_sock < _clients[i]->GetSock())
			{
				max_sock = _clients[i]->GetSock();
			}
		}

		int ret = select(max_sock + 1, &fd_read, nullptr, nullptr, nullptr);//不需要阻塞
		if (ret < 0)
		{
			printf("Server %d select error.\n", (int)_server_sock);
			CloseServer();
			return false;
		}

		for (int i = 0; i < _clients.size(); ++i)
		{
			if (FD_ISSET(_clients[i]->GetSock(), &fd_read))
			{
				int ret = RecvData(_clients[i]);//接收消息
				if (ret == -1)
				{
					auto iter = _clients.begin() + i;
					if (iter != _clients.end())
					{
						if (_pEvent)
						{
							_pEvent->OnNetLeave(_clients[i]);
						}

						//要删delete再erase，会出错！！！！
						delete _clients[i];
						_clients.erase(iter);
					}
				}
			}
		}
	}
}

//接收消息，处理粘包、少包
int CellServer::RecvData(ClientSock* pClient)
{
	//接收客户端消息
	int len = (int)recv(pClient->GetSock(), _recv_buf, RECV_BUF_SIZE, 0);
	//判断客户端是否退出
	if (len <= 0)
	{
		printf("Client %d exit.\n", (int)pClient->GetSock());
		return -1;
	}

	//接收到的数据放入对应客户端消息缓冲区
	memcpy(pClient->GetMsgBuf() + pClient->GetLastPos(), _recv_buf, len);
	pClient->SetLastPos(pClient->GetLastPos() + len);

	//处理粘包、少包问题
	while (pClient->GetLastPos() >= sizeof(Header))
	{
		Header* header = (Header*)pClient->GetMsgBuf();
		if (pClient->GetLastPos() >= header->data_length)
		{
			//更新消息缓冲区
			int temp_last_pos = pClient->GetLastPos() - (header->data_length);
			OnNetMsg(pClient, header);
			memcpy(pClient->GetMsgBuf(), pClient->GetMsgBuf() + (header->data_length), temp_last_pos);
			pClient->SetLastPos(temp_last_pos);
		}
		else
		{
			break;//消息缓冲区剩余数据不够一条完整消息
		}
	}
	return 0;
}

//响应网络数据
void CellServer::OnNetMsg(ClientSock* pClient, Header* header)
{
	_pEvent->OnNetMsg(pClient, header);
}

//消费者取出缓冲队列中的客户端
void CellServer::AddClient(ClientSock* pClient)
{
	//加锁，自解锁
	std::lock_guard<std::mutex> lock(_mutex);
	_clientsBuf.push_back(pClient);
}

void CellServer::StartCellServer()
{
	_pThread = new std::thread(std::mem_fun(&CellServer::OnRun), this);//适配器 std::mem_fun
}

size_t CellServer::GetClientCount()
{
	return size_t(_clients.size()+_clientsBuf.size());
}
