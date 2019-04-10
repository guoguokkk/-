#include"Client.h"
#include<iostream>
using std::cout;
using std::endl;
using std::cin;
Client::Client()
{
	_client_sock = -1;
}

Client::~Client()
{
	CloseClient();
}

//初始化客户端：初始化环境，建立套接字
void Client::InitClient()
{
#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);//LPWSADATA是一个指向WSADATA结构的指针
#endif // _WIN32

	//防止重复初始化
	if (_client_sock < 0)
	{
		_client_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
		if (_client_sock < 0)
			cout << "Client socket error." << endl;
		else
			cout << "Client socket success." << endl;
	}
	else
	{
		CloseClient();//有连接要先关闭
		cout << "Client " << _client_sock << " close old connections." << endl;
	}
}

//连接到服务器
int Client::Connect(const char* ip, const unsigned short port)
{
	//没有初始化需要先初始化
	if (_client_sock <= 0)
	{
		InitClient();
	}

	sockaddr_in server_addr;
#ifdef _WIN32
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	_serv_addr.sin_addr.s_addr = inet_addr(ip);
#endif // _WIN32
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
	int ret = connect(_client_sock, (sockaddr*)& server_addr, sizeof(sockaddr_in));
	if (ret < 0)
		cout << "Client " << _client_sock << " connect error." << endl;
	else
		cout << "Client " << _client_sock << " connect success." << endl;
	return ret;
}

//关闭客户端
void Client::CloseClient()
{
	//防止重复关闭
	if (_client_sock < 0)
	{
		return;
	}
#ifdef _WIN32
	closesocket(_client_sock);
	WSACleanup();
#else
	close(_client_sock);
#endif // _WIN32
	_client_sock = -1;
}

//查询
bool Client::OnRun()
{
	//判断是否正常运行
	if (!IsRun())
		return false;

	fd_set fds_read;//描述符集合，是否可以从这些文件中读取数据
	FD_ZERO(&fds_read);//清零描述符集合
	FD_SET(_client_sock, &fds_read);//客户端加入可读集合

	//设置时间
	timeval time_val;
	time_val.tv_sec = 1;//秒
	time_val.tv_usec = 0;//毫秒
	int ret = select(_client_sock + 1, &fds_read, nullptr, nullptr, &time_val);
	if (ret < 0)
	{
		cout << "Client " << _client_sock << "select task end 1." << endl;
		return false;
	}

	if (FD_ISSET(_client_sock, &fds_read))
	{
		FD_CLR(_client_sock, &fds_read);//清除该客户端

		//客户端退出
		int ret = RecvData();//接收数据
		if (ret == -1)
		{
			cout << "Client " << _client_sock << " select task end 2." << endl;
			CloseClient();
			return false;
		}
	}
	cout << "do other things" << endl;
	return true;
}

//判断是否可以正常运行
bool Client::IsRun()
{
	return _client_sock >= 0;
}

//接收数据
int Client::RecvData()
{
	char recv_buf[4096];//接收缓冲区
	int len = (int)recv(_client_sock, recv_buf, sizeof(Header), 0);//接受数据的头部	
	if (len <= 0)
	{
		cout << "Disconnect from server." << endl;
		return -1;
	}
	Header* header = (Header*)recv_buf;//头部
	recv(_client_sock, recv_buf + sizeof(Header), header->data_length - sizeof(Header), 0);//接受除头部外的其他数据
	OnNetMsg(header, recv_buf);
	return 0;
}

int Client::SendData(Header* header)
{
	//判断消息是否为空
	if (IsRun() && header)
	{
		send(_client_sock, (const char*)header, header->data_length, 0);
		return 0;
	}
	return -1;
}

//处理网络消息
void Client::OnNetMsg(Header* header, char* recv_buf)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* login_result = (LoginResult*)recv_buf;
		cout << "Login result is " << login_result->result << " ,data length is " << login_result->data_length << endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* logout_result = (LogoutResult*)recv_buf;
		cout << "Logout result is " << logout_result->result << " ,data length is " << logout_result->data_length << endl;
	}
	break;
	case CMD_NEW_UER_JOIN:
	{
		NewUserJoin* new_user_join = (NewUserJoin*)recv_buf;
		cout << "New User is " << new_user_join->sock << endl;
	}
	default:
		break;
	}
}
