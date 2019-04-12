#include"Server.h"
#include<iostream>
using std::cout;
using std::endl;
using std::cin;

SOCKET Server::InitServer()
{
#ifdef _WIN32
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32
	if (_server_sock != INVALID_SOCKET)
	{
		cout << "Close old connections." << endl;
		CloseServer();
	}
	_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_sock < 0)
		cout << "Server socket error." << endl;
	else
		cout << "Server " << _server_sock << " socket success." << endl;
	return _server_sock;
}

int Server::Bind(const char* ip, const unsigned short port)
{
	if (_server_sock == INVALID_SOCKET)
		InitServer();

	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
#ifdef _WIN32
	if (ip)
		server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
	else
		server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
#else
	if (ip)
		server_addr.sin_addr.s_addr = inet_addr(ip);
	else
		server_addr.sin_addr.s_addr = INADDR_ANY;
#endif // _WIN32		
	int ret = bind(_server_sock, (const sockaddr*)& server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
		cout << "Server " << _server_sock << " bind error." << endl;
	else
		cout << "Server " << _server_sock << " bind success." << endl;
	return ret;
}

int Server::Listen(int n)
{
	int ret = listen(_server_sock, n);
	if (ret == SOCKET_ERROR)
		cout << "Server " << _server_sock << " listen error." << endl;
	else
		cout << "Server " << _server_sock << " listen success." << endl;
	return ret;
}

SOCKET Server::Accept()
{
	SOCKET client_sock = INVALID_SOCKET;
	sockaddr_in client_addr;
#ifdef _WIN32
	int client_addr_size = sizeof(client_addr);
#else
	socklen_t client_addr_size = sizeof(client_addr);
#endif // _WIN32
	client_sock = accept(_server_sock, (sockaddr*)& client_addr, &client_addr_size);

	if (client_sock==INVALID_SOCKET)
		cout << "Invalid Received Client" << endl;
	else
	{
		NewUserJoin new_user_join;
		SendData2All(&new_user_join);
		_clients.push_back(new ClientSocket(client_sock));//new 返回的是指针
		cout << "New Client join : " << "IP= " << inet_ntoa(client_addr.sin_addr) << endl;
	}
	return client_sock;
}

void Server::CloseServer()
{
	//避免重复关闭
	if (_server_sock == INVALID_SOCKET)
		return;
	
#ifdef _WIN32
	for (int i = 0; i < _clients.size(); ++i)
	{
		closesocket(_clients[i]->GetSockfd());
		delete _clients[i];
	}
	closesocket(_server_sock);
	WSACleanup();
#else
	for (int i = 0; i < _clients.size(); ++i)
	{
		close(_clients[i]->GetSockfd());
		delete _clients[i];
	}
	close(_server_sock);
#endif // _WIN32
	_clients.clear();
}

//select
bool Server::OnRun()
{
	if (!IsRun())//如果没有有效的服务器socket
		return false;

	//windows默认64个，linux默认1000个
	fd_set fds_read; 
	fd_set fds_write;
	fd_set fds_exc;

	//清理集合
	FD_ZERO(&fds_read);
	FD_ZERO(&fds_write);
	FD_ZERO(&fds_exc);

	//将服务器描述符加入集合
	FD_SET(_server_sock, &fds_read);
	FD_SET(_server_sock, &fds_write);
	FD_SET(_server_sock, &fds_exc);

	SOCKET max_sock = _server_sock;
	for (int i = 0; i < _clients.size(); ++i)
	{
		FD_SET(_clients[i]->GetSockfd(), &fds_read);		
		
		if (_clients[i]->GetSockfd() > max_sock)
			max_sock = _clients[i]->GetSockfd();
	}

	timeval time_val;
	time_val.tv_sec = 1;//秒
	time_val.tv_usec = 0;//毫秒
	int ret = select(max_sock + 1, &fds_read, &fds_write, &fds_exc, &time_val);
	if (ret < 0)
	{
		cout << "Server " << _server_sock << "select task end 1." << endl;
		CloseServer();
		return false;
	}

	if (FD_ISSET(_server_sock, &fds_read))
	{
		FD_CLR(_server_sock, &fds_read);
		Accept();
	}
	for (int i = 0; i < FD_SETSIZE; ++i)
	{
		cout << fds_read.fd_array[i] << "\t";
	}
	cout << endl;
	//遍历所有客户端
	for (int i = 0; i < _clients.size(); ++i)
	{
		if (FD_ISSET(_clients[i]->GetSockfd(), &fds_read))
		{
			int ret = RecvData(_clients[i]);//接收客户端数据
			if (ret == -1)
			{//删除已经退出的客户端
				auto iter = _clients.begin() + i;
				if (iter != _clients.end())
				{
					delete _clients[i];//new delete
					_clients.erase(iter);
				}
			}
		}
	}
	//cout << "do other things" << endl;
	return true;
}

bool Server::IsRun()
{
	return _server_sock != INVALID_SOCKET;
}

//接收数据 处理粘包 拆分包
int Server::RecvData(ClientSocket * p_client)
{
	int len = (int)recv(p_client->GetSockfd(), _recv_buf, RECV_BUF_SIZE, 0);//接收头部信息
	if (len <= 0)
	{
		cout << "Client " << p_client->GetSockfd() << " exit." << endl;
		return -1;
	}

	//接收到的数据全部放入对应客户端的消息缓冲区
	memcpy(p_client->GetMsgBuf() + p_client->GetLastPos(), _recv_buf, len);
	p_client->SetLastPos(p_client->GetLastPos() + len);//更改消息缓冲区指针位置

	//该客户端的消息缓冲区数据长度大于头部长度，则可以识别命令
	while (p_client->GetLastPos() >= sizeof(Header))
	{
		Header* header = (Header*)p_client->GetMsgBuf();
		//该客户端的消息缓冲区数据长度大于消息总长度，则可以处理消息
		if (p_client->GetLastPos() >= header->data_length)
		{
			int data_size = p_client->GetLastPos() - header->data_length;
			OnNetMsg(p_client->GetSockfd(), header);

			//删除已经处理的部分
			memcpy(p_client->GetMsgBuf(), p_client->GetMsgBuf() + header->data_length, data_size);
			p_client->SetLastPos(data_size);
		}
		else
			break;//消息不足以处理
	}
	return 0;
}

void Server::OnNetMsg(SOCKET client_sock, Header * header)
{
	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		Login* login = (Login*)header;
		cout << "login: name is " << login->name << " , password is " << login->password << endl;

		LoginResult login_result;
		SendData(client_sock, &login_result);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout* logout = (Logout*)header;
		cout << "Logout: name is " << logout->name << endl;

		LogoutResult logout_result;
		SendData(client_sock, &logout_result);
	}
	break;
	default:
	{
		cout << "Undefined data , data length is " << header->data_length << endl;
	}
	break;
	}
}

int Server::SendData(SOCKET client_sock, Header * header)
{
	if (IsRun() && header)
		return send(client_sock, (const char*)header, header->data_length, 0);	
	return SOCKET_ERROR;
}

void Server::SendData2All(Header * header)
{
	for (int i = 0; i < _clients.size(); ++i)
		SendData(_clients[i]->GetSockfd(), header);
}

