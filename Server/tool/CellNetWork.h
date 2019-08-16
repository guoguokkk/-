#ifndef CELL_NETWORK_H_
#define CELL_NETWORK_H_
#include"../comm/Common.h"
#include"../tool/CellLog.h"
#ifndef _WIN32
#include<fcntl.h>
#include<stdlib.h>
#endif // !_WIN32

//分离网络启动与关闭，避免多个服务器对象启动关闭时出现异常
class CellNetwork
{
private:
	CellNetwork()
	{
#ifdef _WIN32
		//启动Windows socket 2.x环境
		WORD version = MAKEWORD(2, 2);
		WSADATA data;
		WSAStartup(version, &data);
#endif // _WIN32

#ifndef _WIN32
		//if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
		//	return (1);
		//忽略异常信号，默认情况会导致进程终止
		signal(SIGPIPE, SIG_IGN);
#endif // !_WIN32
	}

	~CellNetwork()
	{
#ifdef _WIN32
		WSACleanup();//清除Windows socket环境
#endif // _WIN32
	}

public:
	//初始化
	static void Init()
	{
		static CellNetwork obj;
	}

	static int make_nonblocking(SOCKET fd)
	{
#ifdef _WIN32
		{
			unsigned long nonblocking = 1;
			if (ioctlsocket(fd, FIONBIO, &nonblocking) == SOCKET_ERROR)
			{
				CELLLOG_WARRING("fcntl(%d, F_GETFL)", (int)fd);
				return -1;
			}
		}
#else
		{
			int flags;
			if ((flags = fcntl(fd, F_GETFL, NULL)) < 0)
			{
				CELLLOG_WARRING("fcntl(%d, F_GETFL)", fd);
				return -1;
			}
			if (!(flags & O_NONBLOCK))
			{
				if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) == -1)
				{
					CELLLOG_WARRING("fcntl(%d, F_SETFL)", fd);
					return -1;
				}
			}
		}
#endif
		return 0;
	}

	static int make_reuseaddr(SOCKET fd)
	{
		int flag = 1;
		if (SOCKET_ERROR == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)& flag, sizeof(flag)))
		{
			CELLLOG_WARRING("setsockopt socket<%d> SO_REUSEADDR fail", (int)fd);
			return SOCKET_ERROR;
		}
		return 0;
	}

	static int DestorySocket(SOCKET sockfd)
	{
#ifdef _WIN32
		int ret = closesocket(sockfd);
#else
		int ret = close(sockfd);
#endif // _WIN32

		if (ret < 0)
			CELLLOG_PERROR("destory sockfd<%d>", (int)sockfd);
		return ret;
	}
};

#endif // !CELL_NETWORK_H_
