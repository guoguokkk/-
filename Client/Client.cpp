#include "Client.h"
#include<iostream>
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
		std::cout << "Client " << _client_sock << " close old connections." << std::endl;
		CloseClient();
	}
	_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_client_sock == INVALID_SOCKET)
	{
		std::cout << "Client " << _client_sock << " build socket error." << std::endl;
	}
	else
	{
		std::cout << "Client " << _client_sock << " build socket success." << std::endl;
	}
}

//关闭客户端
void Client::CloseClient()
{

	if (_client_sock == INVALID_SOCKET)
	{
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

int Client::Connect(const char* ip, const short port)
{
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
		std::cout << "Client " << _client_sock << " connect error." << std::endl;
	}
	else
	{
		std::cout << "Client " << _client_sock << " connect success." << std::endl;
	}
	return ret;
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
	time.tv_sec = 1;//秒
	time.tv_usec = 0;
	int ret = select(_client_sock, &fd_read, nullptr, nullptr, &time);
	if (ret < 0)
	{
		std::cout << "Client " << _client_sock << " select error 1." << std::endl;
		CloseClient();
		return false;
	}

	if (FD_ISSET(_client_sock, &fd_read))
	{
		FD_CLR(_client_sock, &fd_read);

		int ret = RecvMessages(_client_sock);//处理收到的消息
		if (ret == -1)
		{
			std::cout << "Client " << _client_sock << " select error 2." << std::endl;
			CloseClient();
			return false;
		}
	}
	return true;
}

int Client::RecvMessages(SOCKET client_sock)
{

	int len = recv(client_sock, _recv_buf, RECV_BUF_SIZE, 0);

	//判断是否断开
	if (len <= 0)
	{
		std::cout << "Client " << client_sock << " disconnect from server." << std::endl;
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
			int temp_last_pos = _last_pos - header->data_length;
			HandleMessages(header);
			memcpy(_msg_buf, _msg_buf + header->data_length, temp_last_pos);
			_last_pos = temp_last_pos;
		}
		else
		{
			break;
		}
	}
	return 0;
}

//处理消息
void Client::HandleMessages(Header* header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* login_result = (LoginResult*)header;
		std::cout << "Login result : cmd= " << login_result->cmd
			<< " , data length= " << login_result->data_length
			<< " , result=" << login_result->result << std::endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* logout_result = (LogoutResult*)header;
		std::cout << "Logout result : cmd= " << logout_result->cmd
			<< " , data length= " << logout_result->data_length
			<< " , result=" << logout_result->result << std::endl;
	}
	break;
	case CMD_ERROR:
	{
		std::cout << "error : cmd= " << header->cmd
			<< " , data length= " << header->data_length
			<< std::endl;
	}
	break;
	case CMD_NEW_USER_JOIN:
	{
		NewUserJoin* new_user_join = (NewUserJoin*)header;
		std::cout << "New user join : cmd= " << new_user_join->cmd
			<< " , data length= " << new_user_join->data_length
			<< " , sock=" << new_user_join->sock << std::endl;
	}
	break;
	default:
	{
		std::cout << "Undefined data : data length= " << header->data_length
			<< std::endl;
	}
	break;
	}
}

//参数：接收对象，要发送的内容
void Client::Send(Header* header)
{
	if (_client_sock != INVALID_SOCKET && header)
	{
		send(_client_sock, (char*)header, header->data_length, 0);
	}
}

