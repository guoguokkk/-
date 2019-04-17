#include "Client.h"
#include<iostream>
#include<stdio.h>
Client::Client()
{
	_client_sock = INVALID_SOCKET;
	memset(_msg_buf, 0, sizeof(_msg_buf));//初始化消息缓冲区
	_last_pos = 0;
	_is_connect = false;
}

Client::~Client()
{
	CloseClient();
	_client_sock = INVALID_SOCKET;
}

void Client::InitClient()
{
#ifdef _WIN32
	//启动 windows 环境
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32

	//建立 socket
	if (_client_sock != INVALID_SOCKET)
	{
		printf("<socket=%d> close old connections.\n", (int)_client_sock);// cout语句不是原子操作
		CloseClient();
	}
	_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_client_sock == INVALID_SOCKET)
	{
		printf("<socket=%d> build socket error.\n", (int)_client_sock);
	}
	else
	{
		//printf("<socket=%d> build socket success.\n", _client_sock);
	}
}

int Client::Connect(const char* ip, const short port)
{
	if (_client_sock == INVALID_SOCKET)
	{
		InitClient();
	}

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
#ifdef _WIN32
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	server_addr.sin_addr.s_addr = inet_addr(ip);
#endif // _WIN32

	int ret = connect(_client_sock, (sockaddr*)& server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		printf("<socket=%d> connect error.\n", (int)_client_sock);
	}
	else
	{
		_is_connect = true;
		//printf("<socket=%d> connect success.\n", _client_sock);
	}
	return ret;
}

//关闭客户端
void Client::CloseClient()
{
	if (_client_sock == INVALID_SOCKET)
	{
		_is_connect = false;
		return;
	}
#ifdef _WIN32
	closesocket(_client_sock);
	WSACleanup();//关闭 windows 环境
#else
	close(_client_sock);
#endif // _WIN32
	_client_sock = INVALID_SOCKET;
}

bool Client::OnRun()
{
	if (_client_sock == INVALID_SOCKET)
	{
		return false;
	}

	fd_set fd_read;
	FD_ZERO(&fd_read);
	FD_SET(_client_sock, &fd_read);
	timeval time;
	time.tv_sec = 0;//秒
	time.tv_usec = 0;
	int ret = select(_client_sock, &fd_read, nullptr, nullptr, &time);
	if (ret < 0)
	{
		printf("<socket=%d> select error 1.\n", (int)_client_sock);
		CloseClient();
		return false;
	}

	if (FD_ISSET(_client_sock, &fd_read))
	{
		FD_CLR(_client_sock, &fd_read);

		int ret = RecvData(_client_sock);//处理收到的消息
		if (ret == -1)
		{
			printf("<socket=%d> select error 2.\n", (int)_client_sock);
			CloseClient();
			return false;
		}
	}
	return true;
}

int Client::RecvData(SOCKET client_sock)
{
	int len = (int)recv(client_sock, _recv_buf, RECV_BUF_SIZE, 0);

	//判断是否断开
	if (len <= 0)
	{
		printf("<socket=%d> disconnect from server.\n", (int)_client_sock);
		return -1;
	}

	//放入消息缓冲区	
	memcpy(_msg_buf + _last_pos, _recv_buf, len);
	_last_pos = _last_pos + len;

	while (_last_pos >= sizeof(Header))
	{
		Header* header = (Header*)_msg_buf;
		if (_last_pos >= header->data_length)
		{
			int temp_last_pos = _last_pos - (header->data_length);
			OnNetMsg(header);
			memcpy(_msg_buf, _msg_buf + (header->data_length), temp_last_pos);
			_last_pos = temp_last_pos;
		}
		else
		{
			break;//消息缓冲区剩余数据不够一条完整消息
		}
	}
	return 0;
}

//处理消息
void Client::OnNetMsg(Header* header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* login_result = (LoginResult*)header;
		/*printf("Login result : socket = %d , data length= %d , result= %d\n",
			(int)_client_sock, login_result->data_length, login_result->result);*/
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
			(int)_client_sock, header->data_length);
	}
	break;
	default:
	{
		printf("Undefined data : socket = %d , data length=  %d\n",
			(int)_client_sock, header->data_length);
	}
	break;
	}
}

//参数：接收对象，要发送的内容
int Client::SendData(Header* header, int len)
{
	int ret = SOCKET_ERROR;
	if (_client_sock != INVALID_SOCKET && header)
	{
		ret = send(_client_sock, (char*)header, len, 0);
		if (ret == SOCKET_ERROR)
		{
			CloseClient();
		}
	}
	return ret;
}

