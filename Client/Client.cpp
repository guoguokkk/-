#include "Client.h"
#include<iostream>
#include<stdio.h>
Client::Client()
{
	_clientSock = INVALID_SOCKET;
	_isConnect = false;
}

Client::~Client()
{
	closeClient();
}

void Client::initClient()
{
#ifdef _WIN32
	//启动 windows 环境
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32

	//建立 socket
	if (_clientSock != INVALID_SOCKET)
	{
		printf("<socket=%d> close old connections.\n", (int)_clientSock);// cout语句不是原子操作
		closeClient();
	}
	_clientSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_clientSock == INVALID_SOCKET)
	{
		printf("<socket=%d> build socket error.\n", (int)_clientSock);
	}
	else
	{
		//printf("<socket=%d> build socket success.\n", _client_sock);
	}
}

int Client::connectToServer(const char* ip, unsigned short port)
{
	if (_clientSock == INVALID_SOCKET)
	{
		initClient();
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
#ifdef _WIN32
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	server_addr.sin_addr.s_addr = inet_addr(ip);
#endif // _WIN32

	int ret = connect(_clientSock, (sockaddr*)& server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		printf("<socket=%d> connect error.\n", (int)_clientSock);
	}
	else
	{
		_isConnect = true;
		//printf("<socket=%d> connect success.\n", _client_sock);
	}
	return ret;
}

//关闭客户端
void Client::closeClient()
{
	if (_clientSock == INVALID_SOCKET)
	{
		_isConnect = false;
		return;
	}
#ifdef _WIN32
	closesocket(_clientSock);
	WSACleanup();//关闭 windows 环境
#else
	close(_clientSock);
#endif // _WIN32
	_clientSock = INVALID_SOCKET;
}

bool Client::onRun()
{
	if (!isRun())
	{
		return false;
	}

	fd_set fd_read;
	FD_ZERO(&fd_read);
	FD_SET(_clientSock, &fd_read);
	timeval time;
	time.tv_sec = 0;//秒
	time.tv_usec = 0;
	int ret = select(_clientSock + 1, &fd_read, nullptr, nullptr, &time);//linux 需要+1
	if (ret < 0)
	{
		printf("<socket=%d> select error 1.\n", (int)_clientSock);
		closeClient();
		return false;
	}

	if (FD_ISSET(_clientSock, &fd_read))
	{
		FD_CLR(_clientSock, &fd_read);

		int ret = recvData(_clientSock);//处理收到的消息
		if (ret == -1)
		{
			printf("<socket=%d> select error 2.\n", (int)_clientSock);
			closeClient();
			return false;
		}
	}
	return true;
}

bool Client::isRun()
{
	return _clientSock != INVALID_SOCKET && _isConnect;
}

int Client::recvData(SOCKET clientSock)
{
	int nLen = (int)recv(clientSock, _recvBuf, RECV_BUF_SIZE, 0);

	//判断是否断开
	if (nLen <= 0)
	{
		printf("<socket=%d> disconnect from server.\n", (int)_clientSock);
		return -1;
	}

	//放入消息缓冲区	
	memcpy(_msgBuf + _lastPos, _recvBuf, nLen);
	_lastPos += nLen;

	while (_lastPos >= sizeof(Header))
	{
		Header* header = (Header*)_msgBuf;
		if (_lastPos >= header->data_length)
		{
			int temp_pos = _lastPos - header->data_length;
			onNetMsg(header);
			memcpy(_msgBuf, _msgBuf + header->data_length, temp_pos);
			_lastPos = temp_pos;
		}
		else
		{
			break;//消息缓冲区剩余数据不够一条完整消息
		}
	}
	return 0;
}

//处理消息
void Client::onNetMsg(Header* header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* login_result = (LoginResult*)header;
		/*printf("Login result : socket = %d , data length= %d , result= %d\n",
			(int)_clientSock, login_result->data_length, login_result->result);*/
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* logout_result = (LogoutResult*)header;
		/*printf("Logout result : socket = %d , data length= %d , result= %d\n",
			(int)_client_sock, logout_result->data_length, logout_result->result);*/
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* new_user_join = (NewUserJoin*)header;
		/*printf("New user join : socket = %d , data length = %d, sock= %d\n",
			(int)_client_sock, new_user_join->data_length, new_user_join->sock);*/
	}
	break;
	case CMD_ERROR:
	{
		printf("error : socket = %d , data length= %d\n",
			(int)_clientSock, header->data_length);
	}
	break;
	default:
	{
		printf("Undefined data : socket = %d , data length=  %d\n",
			(int)_clientSock, header->data_length);
	}
	break;
	}
}

//参数：接收对象，要发送的内容
int Client::sendData(Header* header, int nLen)
{
	int ret = SOCKET_ERROR;
	if (isRun() && header)
	{
		ret = send(_clientSock, (const char*)header, nLen, 0);
		if (ret == SOCKET_ERROR)
		{
			closeClient();
		}
	}
	return ret;
}

