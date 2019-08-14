#ifndef SERVERSELECT_H_
#define SERVERSELECT_H_
#include"../Server/Server.h"
#include"../server/CellServerEpoll.h"

class ServerEpoll :public Server
{
public:
	void StartAllCellServer(int nCellServer)
	{
		Server::StartAllCellServer<CellServerEpoll>(nCellServer);
	}
protected:
	//只负责连接新客户端，参数为线程控制对象 pThread
	void OnRun(CellThread* pThread)
	{

	}
};

#endif // !SERVERSELECT_H_
