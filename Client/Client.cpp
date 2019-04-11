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

void Client::InitClient()
{
#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32

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
		CloseClient();
		cout << "Client " << _client_sock << " close old connections." << endl;
	}
}

int Client::Connect(const char* ip, const unsigned short port)
{
	if (_client_sock <= 0)
	{
		InitClient();
	}

	sockaddr_in server_addr;
#ifdef _WIN32
	server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
	server_addr.sin_addr.s_addr = inet_addr(ip);
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

void Client::CloseClient()
{
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

bool Client::OnRun()
{
	if (!IsRun())
		return false;

	fd_set fds_read;//描述符集合
	FD_ZERO(&fds_read);//清空集合
	FD_SET(_client_sock, &fds_read);//客户端描述符加入集合

	//时间间隔
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
		FD_CLR(_client_sock, &fds_read);//清除客户端

		int ret = RecvData();
		if (ret == -1)
		{
			cout << "Client " << _client_sock << " select task end 2." << endl;
			CloseClient();
			return false;
		}
	}
	//cout << "do other things" << endl;
	return true;
}

bool Client::IsRun()
{
	return _client_sock >= 0;
}

int Client::RecvData()
{
	int len = (int)recv(_client_sock, _recv_buf, RECV_BUF_SIZE, 0);//接收数据到接收缓冲区
	if (len <= 0)
	{
		cout << "Disconnect from server." << endl;
		return -1;
	}

	memcpy(_msg_buf + _last_pos, _recv_buf, len);//把收到的数据拷贝消息缓冲区
	_last_pos += len;//消息缓冲区尾部位置后移
	while (_last_pos >= sizeof(Header))//循环：粘包
	{
		Header* header = (Header*)_msg_buf;
		if (_last_pos >= header->data_length)//少包
		{
			int data_size = _last_pos - header->data_length;//剩余未处理消息缓冲区的数据的长度
			OnNetMsg(header);//处理网络消息
			memcpy(_msg_buf, _recv_buf + header->data_length, data_size);//移除处理完毕的数据
			_last_pos = data_size;//偏移位置
		}
		else
		{
			break;//剩余消息不够一个消息，不处理
		}
	}
	return 0;
}

int Client::SendData(Header* header)
{
	if (IsRun() && header)
	{
		send(_client_sock, (const char*)header, header->data_length, 0);
		return 0;
	}
	return -1;
}

void Client::OnNetMsg(Header* header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN_RESULT:
	{
		LoginResult* login_result = (LoginResult*)_recv_buf;
	//	cout << "Login result is " << login_result->result << " ,data length is " << login_result->data_length << endl;
	}
	break;
	case CMD_LOGOUT_RESULT:
	{
		LogoutResult* logout_result = (LogoutResult*)_recv_buf;
		//cout << "Logout result is " << logout_result->result << " ,data length is " << logout_result->data_length << endl;
	}
	break;
	case CMD_NEW_UER_JOIN:
	{
		NewUserJoin* new_user_join = (NewUserJoin*)_recv_buf;
		//cout << "New User join , it is " << new_user_join->sock<< " ,data length is " << new_user_join->data_length << endl;
	}
	break;
	case CMD_ERROR:
	{
		cout << "Error " << " ,data length is " << header->data_length << endl;
	}
	break;
	default:
	{
		cout << "Undefined data , data length is " << header->data_length << endl;
	}
	break;
	}
}
