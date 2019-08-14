//#include<unistd.h>
//#include<arpa/inet.h>
//#include<string.h>
//#include<cstdio>
//#include<sys/epoll.h>//epoll
//#include<vector>
//#include<thread>
//#include<algorithm>
//
//#define SOCKET int
//#define INVALID_SOCKET  (SOCKET)(~0)
//#define SOCKET_ERROR    (-1)
//
//std::vector<SOCKET> g_clients;
//bool g_bRun = true;
//int msgCount = 0;
//
//void cmdThread()
//{
//	while (true)
//	{
//		char cmdBuf[256] = {};
//		scanf("%s", cmdBuf);
//		if (strcmp(cmdBuf, "exit") == 0)
//		{
//			g_bRun = false;
//			printf("exit thread\n");
//			break;
//		}
//		else
//		{
//			printf("invalid input...\n");
//		}
//	}
//}
//
////添加epoll事件
//int cell_epoll_ctl(int epollFd, int op, SOCKET sockfd, uint32_t events)
//{
//	epoll_event epollEvent;//事件
//	epollEvent.events = events;//客户端加入是可读事件
//	epollEvent.data.fd = sockfd;//关心的socket描述符
//	int ret = epoll_ctl(epollFd, op, sockfd, &epollEvent);//向epoll对象注册需要管理、监听的socket描述符;返回0：操作成功；-1：失败;
//	if (ret == -1)
//	{
//		printf("error, cell_epoll_ctl: epollFd=%d, op=%d, sockfd=%d, events=%u \n", epollFd, op, sockfd, events);
//	}
//	return ret;
//}
//
//char g_BUf[4096] = {};
//int g_nLen = 0;
////接收数据
//int recvData(SOCKET clientSock)
//{
//	g_nLen = (int)recv(clientSock, g_BUf, 4096, 0);
//	return g_nLen;
//}
//
////发送数据
//int sendData(SOCKET clientSock)
//{
//	g_nLen = (int)send(clientSock, g_BUf, g_nLen, 0);
//	return g_nLen;
//}
//
////客户端离开
//void clientLeave(SOCKET clientSock)
//{
//	close(clientSock);
//	printf("client %d exit \n", clientSock);
//	std::vector<SOCKET>::iterator iter = find(g_clients.begin(), g_clients.end(), clientSock);//找到离开的客户端
//	g_clients.erase(iter);
//}
//
//int main()
//{
//	//启动线程
//	std::thread t1(cmdThread);
//	t1.detach();
//
//	//1 建立套接字
//	SOCKET _serverSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
//	if (_serverSock == INVALID_SOCKET)
//		printf("%d socket error\n", _serverSock);
//	else
//		printf("%d socket success\n", _serverSock);
//
//	//2 绑定服务器端口、ip地址
//	sockaddr_in _serverAddr = {};
//	_serverAddr.sin_port = htons(9066);
//	_serverAddr.sin_addr.s_addr = INADDR_ANY;
//	_serverAddr.sin_family = AF_INET;
//
//	int nLen = bind(_serverSock, (sockaddr*)& _serverAddr, sizeof(_serverAddr));
//	if (nLen == SOCKET_ERROR)
//		printf("%d bind error\n", _serverSock);
//	else
//		printf("%d bind success\n", _serverSock);
//
//	//3 监听
//	nLen = listen(_serverSock, 50);
//	if (nLen == SOCKET_ERROR)
//		printf("%d listen error\n", _serverSock);
//	else
//		printf("%d listen success\n", _serverSock);
//
//	//4 建立epoll对象，并添加事件
//	int _epollFd = epoll_create(256);//linux2.6.8以后参数没有意义，由epoll动态管理，最大为filemax(cat /proc/sys/fs/file-max)
//	cell_epoll_ctl(_epollFd, EPOLL_CTL_ADD, _serverSock, EPOLLIN);//添加事件，操作为EPOLL_CTL_ADD，关心的事件为可读事件
//
//	epoll_event _events[256] = {};//接收检测到的事件，最多256个
//
//	while (g_bRun)
//	{
//		int eventsNum = epoll_wait(_epollFd, _events, 256, 1);//0不等待，立即返回;-1一直等待事件发生;正数：等待的时间
//		if (eventsNum < 0)
//		{
//			printf("error, epoll_wait nLen=%d", nLen);
//			break;
//		}
//
//		//nLen个事件发生
//		for (int i = 0; i < eventsNum; ++i)
//		{
//			//服务器端有可读事件，说明有客户端连接
//			if (_events[i].data.fd == _serverSock)
//			{
//				if (_events[i].events & EPOLLIN)
//				{
//					//5 接收客户端连接
//					SOCKET _clientSock = INVALID_SOCKET;
//					sockaddr_in _clientAddr = {};
//					int _clientAddrSize = sizeof(_clientAddr);
//
//					_clientSock = accept(_serverSock, (sockaddr*)& _clientAddr, (socklen_t*)& _clientAddrSize);
//					if (nLen == SOCKET_ERROR)
//					{
//						printf("accept error\n");
//					}
//					else
//					{
//						g_clients.push_back(_clientSock);
//						cell_epoll_ctl(_epollFd, EPOLL_CTL_ADD, _clientSock, EPOLLIN);//添加事件，可读事件
//						printf("accept success: _clientSock=%d, ip=%s \n", _clientSock, inet_ntoa(_clientAddr.sin_addr));
//					}
//					continue;
//				}
//			}
//
//			//客户端发生可读事件
//			if (_events[i].events & EPOLLIN)
//			{
//				printf("EPOLLIN|%d\n", ++msgCount);
//				SOCKET clientSock = _events[i].data.fd;
//
//				//6 接收数据
//				nLen = recvData(clientSock);
//				if (nLen <= 0)
//					clientLeave(clientSock);
//				else
//					printf("id=%d, client sock = %d, length=%d\n", msgCount, clientSock, nLen);
//
//				cell_epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientSock, EPOLLOUT);//更改事件，可写事件
//			}
//
//			//客户端发生可写事件
//			if (_events[i].events & EPOLLOUT)
//			{
//				printf("EPOLLOUT|%d\n", msgCount);
//				SOCKET clientSock = _events[i].data.fd;
//
//				//7 发送数据
//				int ret = sendData(clientSock);
//				if (ret <= 0)
//					clientLeave(clientSock);
//
//				cell_epoll_ctl(_epollFd, EPOLL_CTL_MOD, clientSock, EPOLLIN);//更改事件，可读事件
//			}
//
//			//客户端发生错误事件
//			if (_events[i].events & EPOLLERR)
//			{
//				SOCKET clientSock = _events[i].data.fd;
//				printf("EPOLLERR: id=%d, client sock = %d\n", msgCount, clientSock);
//			}
//
//			//客户端发生挂起事件
//			if (_events[i].events & EPOLLHUP)
//			{
//				SOCKET clientSock = _events[i].data.fd;
//				printf("EPOLLHUP: id=%d, client sock = %d\n", msgCount, clientSock);
//			}
//		}
//
//	}
//
//	//8 关闭
//	for (auto client : g_clients)
//	{
//		close(client);
//	}
//	close(_epollFd);
//	close(_serverSock);
//	printf("EXIT...\n");
//
//	return 0;
//}