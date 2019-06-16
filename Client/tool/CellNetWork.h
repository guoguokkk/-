#ifndef CELL_NETWORK_H_
#define CELL_NETWORK_H_
#include"Message.h"
#include<signal.h>

//分离网络启动与关闭，避免多个服务器对象启动关闭时出现异常
class CellNetwork
{
private:
	CellNetwork()
	{
#ifdef _WIN32
		//启动 windows 环境
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
		WSACleanup();//关闭 windows 环境
#endif // _WIN32
	}

public:
	static void Init()
	{
		static CellNetwork obj;
	}
};

#endif // !CELL_NETWORK_H_

