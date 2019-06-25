#include "Server.h"
#include<iostream>

Server::Server()
{
	_serverSock = INVALID_SOCKET;
	_recvCount = 0;//!一定要初始化
	_clientCount = 0;//!一定要初始化
	_msgCount = 0;//!一定要初始化

	_nSendBufSize = CellConfig::Instance().getInt("nSendBufSize",SEND_BUF_SIZE);
	_nRecvBufSize = CellConfig::Instance().getInt("nRecvBufSize",RECV_BUF_SIZE );
	_nMaxClient = CellConfig::Instance().getInt("nClient",FD_SETSIZE);
}

Server::~Server()
{
	closeServer();
}

//初始化
SOCKET Server::initServer()
{
	CellNetwork::Init();
	if (_serverSock != INVALID_SOCKET)
	{
		CELLLOG_INFO("<socket=%d> close old connections.", (int)_serverSock);
		closeServer();
	}

	_serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (_serverSock == INVALID_SOCKET)
	{
		CELLLOG_ERROR("create socket failed...");
	}
	else
	{
		CellNetwork::make_reuseaddr(_serverSock);
		//CELLLOG_INFO("create socket<%d> success...",(int)_server_sock);
	}
	return _serverSock;
}

//绑定ip地址和端口，参数为ip地址 ip，端口 port
int Server::Bind(const char* ip, unsigned short port)
{
	sockaddr_in server_addr = {};
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
		CELLLOG_ERROR("bind port<%d> failed...", port);
	}
	else
	{
		//CELLLOG_INFO("bind port<%d> success...", port);
	}
	return ret;
}

//监听端口号
int Server::Listen(int n)
{
	int ret = listen(_serverSock, n);
	if (ret == SOCKET_ERROR)
	{
		CELLLOG_ERROR("listen socket<%d> failed...", (int)_serverSock);
	}
	else
	{
		//CELLLOG_INFO("listen port<%d> success...", (int)_server_sock);
	}
	return ret;
}

//接受客户端连接
SOCKET Server::Accept()
{
	SOCKET clientSock = INVALID_SOCKET;
	sockaddr_in client_addr;

#ifdef _WIN32
	int client_addr_size = sizeof(client_addr);
#else
	socklen_t client_addr_size = sizeof(client_addr);
#endif // _WIN32

	clientSock = accept(_serverSock, (sockaddr*)& client_addr, &client_addr_size);
	if (clientSock == INVALID_SOCKET)
	{
		CELLLOG_ERROR("<socket=%d> accept error.", (int)_serverSock);
	}
	else
	{
		//当前连接的客户端数量小于最多可以连接的数目
		if (_clientCount < _nMaxClient)
		{
			CellNetwork::make_reuseaddr(clientSock);
			std::shared_ptr<CellClient> c(new CellClient(clientSock, _nSendBufSize, _nRecvBufSize));
			addClientToCellServer(c);//将新客户端分配给客户数量最少的cellServer
		}
		else
		{
			//超出最大连接数目，直接关闭该客户端
#ifdef _WIN32
			closesocket(clientSock);
#else
			close(clientSock);
#endif // _WIN32
			CELLLOG_WARRING("Accept to nMaxClient");
		}
	}
	return clientSock;
}

//将新客户端分配给客户数量最少的cellServer，参数为客户端 pClient
void Server::addClientToCellServer(std::shared_ptr<CellClient> pClient)
{
	//查找客户数量最少的CellServer消息处理对象
	auto pMinCellServer = _cellServers[0];
	for (auto pCellServer : _cellServers)
	{
		if (pCellServer->getClientCount() < pMinCellServer->getClientCount())
		{
			pMinCellServer = pCellServer;
		}
	}
	pMinCellServer->addClient(pClient);//加入客户端数量最小的消息处理线程	
}


//启动CellServer，参数为CellServer的数量 
void Server::startAllCellServer(int nCellServer)
{
	//启动四个线程来负责消息处理业务
	for (int i = 0; i < nCellServer; ++i)
	{
		auto cellServer = new CellServer(i + 1);//新建一个服务器来负责消息处理业务，参数为id
		_cellServers.push_back(cellServer);
		cellServer->setEventObj(this);//注册网络事件接受对象
		cellServer->startCellServer();//启动消息处理线程
	}

	//启动线程，处理新客户端连接
	_thread.startThread(nullptr,
		[this](CellThread* pThread) {
			onRun(pThread);
		});
}

//关闭
void Server::closeServer()
{
	CELLLOG_INFO("Server.Close begin");
	_thread.closeThread();//关闭新客户端连接处理线程

	//避免重复关闭！
	if (_serverSock != INVALID_SOCKET)
	{
		for (auto cs : _cellServers)
		{
			delete cs;
		}
		_cellServers.clear();

#ifdef _WIN32
		closesocket(_serverSock);
#else
		close(_serverSock);
#endif // _WIN32
		_serverSock = INVALID_SOCKET;
	}
	CELLLOG_INFO("Server.Close end");
}

void Server::onNetJoin(std::shared_ptr<CellClient> pClient)
{
	++_clientCount;
}

void Server::onNetLeave(std::shared_ptr<CellClient> pClient)
{
	--_clientCount;
}

void Server::onNetMsg(CellServer* pCellServer, std::shared_ptr<CellClient> pClient, netmsg_DataHeader* header)
{
	++_msgCount;
}

void Server::onNetRecv(std::shared_ptr<CellClient> pClient)
{
	++_recvCount;
}

//只负责连接新客户端，参数为线程控制对象 pThread
void Server::onRun(CellThread* pThread)
{
	CellFDSet fd_read;//可读集合

	//判断负责处理新客户端连接的线程是否在运行
	while (pThread->isRun())
	{
		time4Msg();//计算并输出每秒收到的网络消息

		fd_read.zero();//清理集合
		fd_read.add(_serverSock); //将服务器端描述符加入集合

		timeval time;
		time.tv_sec = 0;//秒
		time.tv_usec = 1;
		int ret = select(_serverSock + 1, fd_read.fdset(), nullptr, nullptr, &time);
		if (ret < 0)
		{
			CELLLOG_ERROR("<socket=%d> select error.", (int)_serverSock);
			pThread->exitThread();
			break;
		}
		//服务器端描述符在集合中，则连接新客户端
		if (fd_read.has(_serverSock))
		{
			Accept();
		}
	}
}

//计算并输出每秒收到的网络消息
void Server::time4Msg()
{
	auto t1 = _tTime.getElapsedSecond();
	if (t1 >= 1.0)
	{
		CELLLOG_INFO("thread<%d>,time<%lf>,socket<%d>,clients<%d>,recvCount<%d>,msgCount<%d>",
			(int)_cellServers.size(), (double)t1, (int)_serverSock,
			(int)(_clientCount), (int)(_recvCount), (int)_msgCount);
		_recvCount = 0;
		_msgCount = 0;
		_tTime.update();
	}
}

