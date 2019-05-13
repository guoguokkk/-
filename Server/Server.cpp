#include "Server.h"
#include<iostream>
Server::Server()
{
	_serverSock = INVALID_SOCKET;
	_recvCount = 0;//!一定要初始化
	_clientCount = 0;//!一定要初始化
	_msgCount = 0;//!一定要初始化
}

Server::~Server()
{
	closeServer();
}

SOCKET Server::initServer()
{
#ifdef _WIN32
	//启动 windows 环境
	WORD version = MAKEWORD(2, 2);
	WSADATA data;
	WSAStartup(version, &data);
#endif // _WIN32

	//建立 socket
	if (_serverSock != INVALID_SOCKET)
	{
		printf("<socket=%d> close old connections.\n", (int)_serverSock);
		closeServer();
	}
	_serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_serverSock == INVALID_SOCKET)
	{
		printf("<socket=%d> socket error.\n", (int)_serverSock);
	}
	else
	{
		//printf("<socket=%d> socket success.\n",(int)_server_sock);
	}
	return _serverSock;
}

//!注意参数
int Server::Bind(const char* ip, unsigned short port)
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
		server_addr.sin_addr.s_addr = inet_addr(ip);
	}
	else
	{
		server_addr.sin_addr.s_addr = INADDR_ANY;
	}
#endif // _WIN32

	int ret = bind(_serverSock, (sockaddr*)& server_addr, sizeof(server_addr));
	if (ret == SOCKET_ERROR)
	{
		printf("<socket=%d> bind error.\n", (int)_serverSock);
	}
	else
	{
		//printf("<socket=%d> bind success.\n", (int)_server_sock);
	}
	return ret;
}

int Server::Listen(int n)
{
	int ret = listen(_serverSock, n);
	if (ret == SOCKET_ERROR)
	{
		printf("<socket=%d> listen error.\n", (int)_serverSock);
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
	client_sock = accept(_serverSock, (sockaddr*)& client_addr, &client_addr_size);

	if (client_sock == INVALID_SOCKET)
	{
		printf("<socket=%d> accept error.\n", (int)_serverSock);
	}
	else
	{
		//NewUserJoin new_user_join;
		//SendToAll(&new_user_join);

		//加入对象池之后需要调整make_shared，否则只会在内存池中分配内存，而不进入对象池
		//addClientToCellServer(std::make_shared<CellClient>(client_sock));
		std::shared_ptr<CellClient> c(new CellClient(client_sock));
		addClientToCellServer(c);

		//printf("New client %d join.\n", (int)client_sock);
	}
	return client_sock;
}

void Server::addClientToCellServer(std::shared_ptr<CellClient> pClient)
{
	//查找客户数量最少的CellServer消息处理对象
	auto pMinServer = _cellServers[0];
	for (auto pCellServer : _cellServers)
	{
		if (pMinServer->getClientCount() > pCellServer->getClientCount())
		{
			pMinServer = pCellServer;
		}
	}
	pMinServer->addClient(pClient);//加入客户端数量最小的消息处理线程	
	onNetJoin(pClient);
}

void Server::startServer(int n_cellServer)
{
	//启动四个线程来负责消息处理业务
	for (int i = 0; i < n_cellServer; ++i)
	{
		auto cell_server = new CellServer(_serverSock);//新建一个服务器来负责消息处理业务
		_cellServers.push_back(cell_server);
		cell_server->setEventObj(this);//注册网络事件接受对象
		cell_server->startCellServer();//启动消息处理线程
	}
}

void Server::closeServer()
{
	//避免重复关闭！
	if (_serverSock == INVALID_SOCKET)
	{
		return;
	}

#ifdef _WIN32
	closesocket(_serverSock);
	WSACleanup();//关闭 windows 环境
#else
	close(_serverSock);
#endif // _WIN32
}

//只负责连接新客户端，有其他线程负责消息处理
bool Server::onRun()
{
	if (!isRun())
	{
		return false;
	}

	time4Msg();
	fd_set fd_read;
	FD_ZERO(&fd_read);
	FD_SET(_serverSock, &fd_read);

	timeval time;
	time.tv_sec = 0;//秒
	time.tv_usec = 10;
	int ret = select(_serverSock + 1, &fd_read, nullptr, nullptr, &time);
	if (ret < 0)
	{
		printf("<socket=%d> select error.\n", (int)_serverSock);
		closeServer();
		return false;
	}

	if (FD_ISSET(_serverSock, &fd_read))
	{
		FD_CLR(_serverSock, &fd_read);
		Accept();
		return true;
	}
	return false;
}

bool Server::isRun()
{
	return _serverSock != INVALID_SOCKET;
}

void Server::time4Msg()
{
	auto t1 = _tTime.getElapsedSecond();
	if (t1 >= 1.0)
	{
		printf("thread<%d>,time<%lf>,socket<%d>,clients<%d>,msgCount<%d>,recvCount<%d>\n",
			(int)_cellServers.size(), (double)t1, (int)_serverSock,
			(int)(_clientCount), (int)_msgCount, (int)(_recvCount));
		_recvCount = 0;
		_msgCount = 0;
		_tTime.update();
	}
}

void Server::onNetJoin(std::shared_ptr<CellClient> pClient)
{
	++_clientCount;
}

//cellServer 4 多个线程触发 不安全
//如果只开启1个cellServer就是安全的
void Server::onNetLeave(std::shared_ptr<CellClient> pClient)
{
	--_clientCount;
}

void Server::onNetMsg(CellServer* pCellServer, std::shared_ptr<CellClient> pClient, Header* header)
{
	++_msgCount;
}

void Server::onNetRecv(std::shared_ptr<CellClient> pClient)
{
	++_recvCount;
}
