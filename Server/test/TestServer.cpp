#include"../memory/MemoryAlloc.h"
#include"../memory/ObjectPool.h"
#include"../server/Server.h"
#include"../tool/CellLog.h"
#include"../tool/Message.h"
#include"MyServer.h"
#include<iostream>
#include<thread>
#include<cstring>

//第四个参数为默认参数，第五个参数为要获取的参数名字
const char* argToStr(int argc, char* args[], int index, const char* def, const char* argName)
{
	//没有输入参数
	if (index >= argc)
	{
		CELLLOG_ERROR("argToStr, index=%d|argc=%d|argName=%s", index, argc, argName);
	}
	else
	{
		def = args[index];
	}
	CELLLOG_INFO("%s=%s", argName, def);
	return def;
}

//第四个参数为默认参数，第五个参数为要获取的参数名字
int argToInt(int argc, char* args[], int index, int def, const char* argName)
{
	//没有输入参数
	if (index >= argc)
	{
		CELLLOG_ERROR("argToInt, index=%d|argc=%d|argName=%s", index, argc, argName);
	}
	else
	{
		def = atoi(args[index]);//atoi()的参数是 const char*，stoi()的参数是const string*
	}
	CELLLOG_INFO("%s=%d", argName, def);
	return def;
}

int main(int argc, char* args[])
{
	const char* strIP = argToStr(argc, args, 1, "any", "strIP");
	uint16_t nPort = argToInt(argc, args, 2, 8099, "nPort");
	int nThread = argToInt(argc, args, 3, 1, "nThread");//线程数量
	int nClient = argToInt(argc, args, 4, 1, "nClient");//客户端数量
	
	if (strcmp(strIP, "any") == 0)
	{
		strIP = nullptr;
	}

	CellLog::Instance().setLogPath("../../serverLog", "w");
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

	Sleep(1);
	return 0;
}