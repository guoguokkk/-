#include "Server.h"
#include<iostream>
Server::~Server()
{
	CloseServer();
	_server_sock = INVALID_SOCKET;
}

void Server::InitServer()
{
#ifdef _WIN32
	//启动 windows 环境
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32

	//建立 socket
	if (_server_sock != INVALID_SOCKET)
	{
		std::cout << "Server " << _server_sock << " close old connections." << std::endl;
		CloseServer();
	}
	_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_sock == INVALID_SOCKET)
	{
		std::cout << "Server " << _server_sock << " socket error." << std::endl;
	}
	else
	{
		std::cout << "Server " << _server_sock << " socket success." << std::endl;
	}
}

void Server::CloseServer()
{
	//避免重复关闭！
	if (_server_sock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	//关闭每个客户端！
	for (int i = 0; i < _client_groups.size(); ++i)
	{
		closesocket(_client_groups[i]->GetSock());
		delete _client_groups[i];
	}
	closesocket(_server_sock);
	WSACleanup();//关闭 windows 环境
#else
	//关闭每个客户端！
	for (int i = 0; i < _client_groups.size(); ++i)
	{
		close(_client_groups[i]->GetSock());
		delete _client_groups[i];
	}
	close(_server_sock);
#endif // _WIN32
}

//!注意参数
void Server::Bind(const char* ip, const short port)
{
	sockaddr_in server_addr;
	server_addr.sin_family = AF_INET;
	server_addr.sin_port = htons(port);
#ifdef _WIN32
	if (ip)
	{
		server_addr.sin_addr.S_un.S_addr = inet_addr(ip);
	}
	else
	{
		server_addr.sin_addr.S_un.S_addr = INADDR_ANY;
	}
#else
	if (ip)
	{
		_server_addr.sin_addr.s_addr = inet_addr(ip);
	}
	else
	{
		_server_addr.sin_addr.s_addr = INADDR_ANY;
	}
#endif // _WIN32	
	int ret = bind(_server_sock, (sockaddr*)& server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		std::cout << "Server " << _server_sock << " bind error." << std::endl;
	}
	else
	{
		std::cout << "Server " << _server_sock << " bind success." << std::endl;
	}
}

void Server::Listen(int n)
{
	int ret = listen(_server_sock, 5);
	if (ret == SOCKET_ERROR)
	{
		std::cout << "Server " << _server_sock << " listen error." << std::endl;
	}
	else
	{
		std::cout << "Server " << _server_sock << " listen success." << std::endl;
	}
}

void Server::Accept()
{
	SOCKET client_sock = INVALID_SOCKET;
	sockaddr_in client_addr;
#ifdef _WIN32
	int client_addr_size = sizeof(client_addr);
#else
	socksize_t client_addr_size = sizeof(client_addr);
#endif // _WIN32
	client_sock = accept(_server_sock, (sockaddr*)& client_addr, &client_addr_size);
	if (client_sock == INVALID_SOCKET)
	{
		std::cout << "Server " << _server_sock << " accept error." << std::endl;
	}
	else
	{
		NewUserJoin new_user_join;
		SendToAll(&new_user_join);
		_client_groups.push_back(new ClientSock(client_sock));
		std::cout << "New client  " << client_sock << " join." << std::endl;
	}
}

bool Server::OnRun()
{
	if (_server_sock == INVALID_SOCKET)
	{
		return false;
	}

	fd_set fd_read;
	FD_ZERO(&fd_read);
	FD_SET(_server_sock, &fd_read);

	SOCKET max_sock = _server_sock;
	for (int i = 0; i < _client_groups.size(); ++i)
	{
		FD_SET(_client_groups[i]->GetSock(), &fd_read);
		if (max_sock < _client_groups[i]->GetSock())
		{
			max_sock = _client_groups[i]->GetSock();
		}
	}

	timeval time;
	time.tv_sec = 1;//秒
	time.tv_usec = 0;
	int ret = select(_server_sock, &fd_read, nullptr, nullptr, &time);
	if (ret < 0)
	{
		std::cout << "Server " << _server_sock << " select error." << std::endl;
		CloseServer();
		return false;
	}

	if (FD_ISSET(_server_sock, &fd_read))
	{
		FD_CLR(_server_sock, &fd_read);
		Accept();
	}
	for (int i = 0; i < _client_groups.size(); ++i)
	{
		if (FD_ISSET(_client_groups[i]->GetSock(), &fd_read))
		{

			int ret = RecvMessages(_client_groups[i]);//接收消息
			if (ret == -1)
			{
				//要删delete再erase，会出错！！！！
				delete _client_groups[i];
				_client_groups.erase(_client_groups.begin() + i);
			}
		}
	}
	return true;
}

void Server::SendToAll(Header * header)
{
	for (int i = 0; i < _client_groups.size(); ++i)
	{
		SendToOne(_client_groups[i]->GetSock(), header);
	}
}

//参数：接收对象，要发送的内容
void Server::SendToOne(SOCKET client_sock, Header * header)
{
	if (_server_sock != INVALID_SOCKET && header)
	{
		send(client_sock, (char*)header, header->data_length, 0);
	}
}

int Server::RecvMessages(ClientSock * client)
{
	//接收客户端消息
	int len = recv(client->GetSock(), _recv_buf, RECV_BUF_SIZE, 0);
	//判断客户端是否退出
	if (len <= 0)
	{
		std::cout << "Client " << client->GetSock() << " exit." << std::endl;
		return -1;
	}

	//接收到的数据放入对应客户端消息缓冲区
	memcpy(client->GetMsgBuf() + client->GetLastPos(), _recv_buf, len);
	client->SetLastPos(client->GetLastPos() + len);

	//处理粘包、少包问题
	while (client->GetLastPos() >= sizeof(Header))
	{
		Header* header = (Header*)client->GetMsgBuf();
		if (client->GetLastPos() >= header->data_length)
		{
			//更新消息缓冲区
			int temp_last_pos = client->GetLastPos() - header->data_length;
			HandleMessages(client->GetLastPos(), header);
			memcpy(client->GetMsgBuf(), client->GetMsgBuf() + header->data_length, temp_last_pos);
			client->SetLastPos(temp_last_pos);
		}
		else
		{
			break;
		}
	}
	return 0;
}

void Server::HandleMessages(SOCKET client_sock, Header * header)
{
	++_recv_count;
	auto t1 = _tTime.GetElapsedSecond();
	if (t1 >= 1.0)
	{
		std::cout << "time:\t" << t1 << "\t\tclients\t" << _client_groups.size()
			<< "\tsocket:\t" << _server_sock << "\t,recv count:\t" << _recv_count << std::endl;
		_recv_count = 0;
		_tTime.Update();
	}

	switch (header->cmd)
	{
	case CMD_LOGIN:
	{
		Login* login = (Login*)header;
		/*std::cout << "Login : cmd= " << login->cmd
			<< " , data length= " << login->data_length
			<< " , user name=" << login->name
			<< " , password=" << login->password << std::endl;*/

		LoginResult login_result;
		recv(client_sock, (char*)& login_result, login_result.data_length, 0);
	}
	break;
	case CMD_LOGOUT:
	{
		Logout* logout = (Logout*)header;
		std::cout << "Logout : cmd= " << logout->cmd
			<< " , data length= " << logout->data_length
			<< " , user name=" << logout->name << std::endl;

		LogoutResult logout_result;
		recv(client_sock, (char*)& logout_result, logout_result.data_length, 0);
	}
	break;
	case CMD_ERROR:
	{
		std::cout << "error : cmd= " << header->cmd
			<< " , data length= " << header->data_length
			<< std::endl;
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
