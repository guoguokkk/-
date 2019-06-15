#include"../memory/MemoryAlloc.h"
#include"../memory/ObjectPool.h"
#include"../server/Server.h"
#include"../tool/CellLog.h"
#include"../tool/Message.h"
#include"MyServer.h"
#include<iostream>
#include<thread>

#define THREAD_COUNT 1//线程数量

int main()
{
	CellLog::Instance().setLogPath("../../serverLog", "w");
	MyServer server;
	server.initServer();
	server.Bind(nullptr, PORT);
	server.Listen(64);
	server.startServer(THREAD_COUNT);//启动服务器，输入服务器的数量


	//在主线程中等待用户输入命令
	while (true)
	{
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (0 == strcmp(cmdBuf, "exit"))
		{
			server.closeServer();
			break;
		}
		else
		{
			CELLLOG_INFO("undefine cmd\n");
		}
	}

	std::cout << "EXIT...." << std::endl;

	Sleep(1);
	return 0;
}