#include "Server.h"
#include<iostream>
Server::~Server()
{
	CloseServer();
}

SOCKET Server::InitServer()
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
		printf("<socket=%d> close old connections.\n",(int)_server_sock);
		CloseServer();
	}
	_server_sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_server_sock == INVALID_SOCKET)
	{
		printf("<socket=%d> socket error.\n", (int)_server_sock);
	}
	else
	{
		//printf("<socket=%d> socket success.\n",(int)_server_sock);
	}
	return _server_sock;
}

//!注意参数
int Server::Bind(const char* ip, const short port)
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
		printf("<socket=%d> bind error.\n", (int)_server_sock);
	}
	else
	{
		//printf("<socket=%d> bind success.\n", (int)_server_sock);
	}
	return ret;
}

int Server::Listen(int n)
{
	int ret = listen(_server_sock, 5);
	if (ret == SOCKET_ERROR)
	{
		printf("<socket=%d> listen error.\n", (int)_server_sock);
	}
	else
	{
		//printf("<socket=%d> listen success.\n", (int)_server_sock);
	}
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

	if (client_sock == INVALID_SOCKET)
	{
		printf("<socket=%d> accept error.\n", (int)_server_sock);
	}
	else
	{
		//NewUserJoin new_user_join;
		//SendToAll(&new_user_join);
		AddClientToCellServer(new ClientSock(client_sock));
		//printf("New client %d join.\n", (int)client_sock);
	}
	return client_sock;
}

void Server::AddClientToCellServer(ClientSock* p_client)
{
	//查找客户数量最少的CellServer消息处理对象
	auto p_min_server = _cell_servers[0];
	for (auto p_cell_server : _cell_servers)
	{
		if (p_min_server->GetClientCount() > p_cell_server->GetClientCount())
		{
			p_min_server = p_cell_server;
		}
	}
	p_min_server->AddClient(p_client);//加入客户端数量最小的消息处理线程	
	OnNetJoin(p_client);
}

void Server::StartServer(int n_cell_server)
{
	//启动四个线程来负责消息处理业务
	for (int i = 0; i < n_cell_server; ++i)
	{
		auto cell_server = new CellServer(_server_sock);//新建一个服务器来负责消息处理业务
		_cell_servers.push_back(cell_server);
		cell_server->SetEventObj(this);//注册网络事件接受对象
		cell_server->StartCellServer();//启动消息处理线程
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
	closesocket(_server_sock);
	WSACleanup();//关闭 windows 环境
#else
	close(_server_sock);
#endif // _WIN32
}

//只负责连接新客户端，有其他线程负责消息处理
bool Server::OnRun()
{
	if (_server_sock == INVALID_SOCKET)
	{
		return false;
	}

	Time4Msg();
	fd_set fd_read;
	FD_ZERO(&fd_read);
	FD_SET(_server_sock, &fd_read);

	timeval time;
	time.tv_sec = 0;//秒
	time.tv_usec = 10;
	int ret = select(_server_sock + 1, &fd_read, nullptr, nullptr, &time);
	if (ret < 0)
	{
		printf("<socket=%d> select error.\n",(int)_server_sock);
		CloseServer();
		return false;
	}

	if (FD_ISSET(_server_sock, &fd_read))
	{
		FD_CLR(_server_sock, &fd_read);
		Accept();
		return true;
	}
	return true;
}

void Server::Time4Msg()
{
	auto t1 = _tTime.GetElapsedSecond();
	if (t1 >= 1.0)
	{
		printf("thread<%d>,time<%lf>,socket<%d>,clients<%d>,recvCount<%d>\n",
			(int)_cell_servers.size(),(double) t1, (int)_server_sock, (int)(_client_count), (int)(_recv_count));
		_recv_count = 0;
		_tTime.Update();
	}
}

void Server::OnNetJoin(ClientSock * p_client)
{
	++_client_count;
}

//cellServer 4 多个线程触发 不安全
//如果只开启1个cellServer就是安全的
void Server::OnNetLeave(ClientSock * p_client)
{
	--_client_count;
}

void Server::OnNetMsg(ClientSock * p_client, Header * header)
{
	++_recv_count;
}
