#include"../memory/MemoryAlloc.h"
#include"../memory/ObjectPool.h"
#include"../comm/Common.h"
#include"../test/MyServerSelect.h"
#include<iostream>
#include<thread>
#include<cstring>

#ifdef __linux__
#include"../test/MyServerEpoll.h"
#endif // __linux__

int main(int argc, char* args[]) {
	//设置运行日志名称
	//CellLog::Instance().setLogPath("F:/AA/guoguokkk/log/serverLog", "w", false);
	CellLog::Instance().setLogPath("./serverLog", "w", false);

	//解析配置信息
	CellConfig::Instance().Init(argc, args);
	const char* strIP = CellConfig::Instance().getStr("strIP", "any");
	uint16_t nPort = CellConfig::Instance().getInt("nPort", 8099);
	int nThread = CellConfig::Instance().getInt("nThread", 1);//线程数量
	int nClient = CellConfig::Instance().getInt("nClient", 10000);//客户端数量
	if (strcmp(strIP, "any") == 0) {
		strIP = nullptr;
	}

	//启动服务器端
#ifdef _WIN32
	MyServerSelect server;
#else
	MyServerEpoll server;
#endif

	server.InitServer(AF_INET6);
	server.Bind(strIP, nPort);
	server.Listen(64);
	server.StartAllCellServer(nThread);//启动所有的消息处理CellServer线程(nThread个)和新客户端连接线程

	//在主线程中等待用户输入命令
	while (true) {
		char cmdBuf[256] = {};
		scanf("%s", cmdBuf);
		if (strcmp(cmdBuf, "exit") == 0) {
			server.CloseServer();
			break;
		}
		else {
			CELLLOG_INFO("undefine cmd");
		}
	}

	CELLLOG_INFO("EXIT....");
	return 0;
}