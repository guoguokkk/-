#ifndef CELL_NETWORK_H_
#define CELL_NETWORK_H_
#include"../tool/Common.h"
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
		WSADATA getData;
		WSAStartup(version, &getData);
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
	static void Init()
	{
		static CellNetwork obj;
	}

	static int make_nonblocking(SOCKET fd)
	{
#ifdef _WIN32
		{
			unsigned long nonblocking = 1;

			//一个标识套接口的描述字、对套接口s的操作命令、指向cmd命令所带参数的指针
			//FIONBIO：允许或禁止套接口s的非阻塞模式
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

		//标识一个套接口的描述字、选项定义的层次、需设置的选项、指针，指向存放选项待设置的新值的缓冲区、optval缓冲区长度。
		//SO_REUSEADDR，打开或关闭地址复用功能，此处为打开
		//如果想要在套接字级别上设置选项，就必须把level设置为 SOL_SOCKET
		if (SOCKET_ERROR == setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (const char*)& flag, sizeof(flag)))
		{
			CELLLOG_WARRING("setsockopt socket<%d> SO_REUSEADDR fail", (int)fd);
			return SOCKET_ERROR;
		}
		return 0;
	}
};

#endif // !CELL_NETWORK_H_
