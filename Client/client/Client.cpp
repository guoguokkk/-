#include "Client.h"
#include<iostream>
#include<stdio.h>
#include"../tool/CellNetWork.h"
#include"../tool/CellLog.h"

Client::Client()
{
	_isConnect = false;
	_addressFamily = AF_INET;
}

Client::~Client()
{
	closeClient();
}

//初始化客户端，af表示ipv4或者ipv5
SOCKET Client::initClient(int af, int sendSize, int recvSize)
{
	_addressFamily = af;
	CellNetwork::Init();

	if (_pClient)
	{
		CELLLOG_INFO("<socket=%d> close old connections.", _pClient->GetSockfd());
		closeClient();
	}

	//AF_INET ipv4，AF_INET6 ipv6
	SOCKET clientSock = socket(af, SOCK_STREAM, IPPROTO_TCP);
	if (clientSock == INVALID_SOCKET)
	{
		CELLLOG_ERROR("<socket=%d> build socket error.", (int)clientSock);
	}
	else
	{
		CellNetwork::make_reuseaddr(clientSock);
		//CELLLOG_INFO("<socket=%d> build socket success.", _client_sock);
		_pClient = new CellClient(clientSock, sendSize, recvSize);
	}
	return clientSock;
}

//连接服务器
int Client::connectToServer(const char* ip, unsigned short port)
{
	if (_pClient == nullptr)
		return SOCKET_ERROR;

	int ret = SOCKET_ERROR;
	if (_addressFamily == AF_INET) {//ipv4
		sockaddr_in server_addr;
		server_addr.sin_family = AF_INET;
		server_addr.sin_port = htons(port);
#ifdef _WIN32
		server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		server_addr.sin_addr.s_addr = inet_addr(ip);
#endif // _WIN32

		ret = connect(_pClient->GetSockfd(), (sockaddr*)& server_addr, sizeof(server_addr));
	}
	else {//ipv6
		sockaddr_in6 server_addr;
		server_addr.sin6_family = AF_INET6;
		server_addr.sin6_port = htons(port);
		inet_pton(AF_INET6, ip, &server_addr.sin6_addr);
		ret = connect(_pClient->GetSockfd(), (sockaddr*)& server_addr, sizeof(server_addr));
	}

	if (ret == SOCKET_ERROR)
	{
		CELLLOG_ERROR("<socket=%d> connect error.", (int)_pClient->GetSockfd());
	}
	else
	{
		_isConnect = true;
		//CELLLOG_INFO("<socket=%d> connect success.", _pClient->getSockfd());
	}
	return ret;
}

//关闭客户端
void Client::closeClient()
{
	if (_pClient)
	{
		delete _pClient;
		_pClient = nullptr;
	}
	_isConnect = false;
}

//处理网络消息，参数为阻塞时间 microseconds
bool Client::onRun(int microseconds)
{
	if (isRun())
	{
		SOCKET clientSock = _pClient->GetSockfd();

		//计算可读集合
		_fdRead.Zero();
		_fdRead.Add(clientSock);

		//计算可写集合
		_fdWrite.Zero();
		timeval time = { 0,microseconds };
		int ret = 0;
		if (_pClient->NeedWrite())
		{
			_fdWrite.Add(clientSock);
			ret = select(clientSock + 1, _fdRead.GetFdSet(), _fdWrite.GetFdSet(), nullptr, &time);//linux 需要+1
		}
		else
		{
			ret = select(clientSock + 1, _fdRead.GetFdSet(), nullptr, nullptr, &time);//linux 需要+1
		}

		if (ret < 0)
		{
			CELLLOG_INFO("<socket=%d> select error 1.", (int)clientSock);
			closeClient();
			return false;
		}

		if (_fdRead.Has(clientSock))
		{
			int ret = recvData(clientSock);//处理收到的消息
			if (ret == -1)
			{
				CELLLOG_ERROR("<socket=%d>OnRun.select RecvData exit", (int)clientSock);
				closeClient();
				return false;
			}
		}

		if (_fdWrite.Has(clientSock))
		{
			int ret = _pClient->SendDataReal();//处理收到的消息
			if (ret == -1)
			{
				CELLLOG_INFO("<socket=%d> select error 2.", (int)clientSock);
				closeClient();
				return false;
			}
		}
		return true;
	}
	return false;
}

bool Client::isRun()
{
	return _pClient && _isConnect;
}

//接受服务器端数据
int Client::recvData(SOCKET clientSock)
{
	if (isRun())
	{
		int nLen = _pClient->RecvData();
		if (nLen > 0)
		{
			//判断是否有消息需要处理
			while (_pClient->HasMsg())
			{
				onNetMsg(_pClient->GetFrontMsg());//处理第一个消息
				_pClient->PopFrontMsg();//移除第一个数据
			}
		}
		return nLen;
	}
	return 0;
}

//发送数据
int Client::sendData(netmsg_DataHeader* header)
{
	if (isRun())
		return _pClient->SendData(header);
	return SOCKET_ERROR;
}

int Client::sendData(const char* pData, int len)
{
	if (isRun())
		return _pClient->SendData(pData, len);
	return SOCKET_ERROR;
}

