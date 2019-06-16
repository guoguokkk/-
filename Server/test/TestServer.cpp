#include"../memory/MemoryAlloc.h"
#include"../memory/ObjectPool.h"
#include"../server/Server.h"
#include"../tool/CellLog.h"
#include"../tool/Message.h"
#include"MyServer.h"
#include<iostream>
#include<thread>
#include<cstring>
#include"../tool/CellConfig.h"

int main(int argc, char* args[])
{
	CellConfig::Instance().Init(argc, args);
	const char* strIP = CellConfig::Instance().getStr("any", "strIP");
	uint16_t nPort = CellConfig::Instance().getInt(8099, "nPort");
	int nThread = CellConfig::Instance().getInt(1, "nThread");//线程数量
	int nClient = CellConfig::Instance().getInt(1, "nClient");//客户端数量
	   
	if (strcmp(strIP, "any") == 0)
	{
		strIP = nullptr;
	}

	CellLog::Instance().setLogPath("../../log/serverLog", "w");
	MyServer server;
	server.initServer();
	server.Bind(strIP, nPort);
	server.Listen(64);
	server.startServer(nThread);//启动服务器，输入服务器端线程的数量

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
			CELLLOG_INFO("undefine cmd");
		}
	}

	std::cout << "EXIT...." << std::endl;

	//#ifdef _WIN32
	//	Sleep(1);
	//#endif // _WIN32

	return 0;
}