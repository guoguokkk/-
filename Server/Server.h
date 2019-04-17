#ifndef SERVER_H_
#define SERVER_H_

#ifdef _WIN32
#include"../Test/Message.h"
#include"../Test/TimeStamp.h"
#else
#include"Message.h"
#include"TimeStamp.h"
#endif // _WIN32

#include"INetEvent.h"
#include"ClientSocket.h"
#include"CellServer.h"
#include<vector>

class Server :public INetEvent
{
public:
	Server()
	{
		_serverSock = INVALID_SOCKET;
		_recvCount = 0;//!一定要初始化
		_clientCount = 0;//!一定要初始化
	}
	~Server();
	SOCKET initServer();//初始化服务器
	int Bind(const char* ip, unsigned short port);//绑定ip地址和端口
	int Listen(int n);//监听客户端
	SOCKET Accept();//接收客户端
	void addClientToCellServer(ClientSock* pClient);
	void startServer(int n_cellServer);
	void closeServer();//关闭服务器	   	 
	bool onRun();//select
	bool isRun();
	void time4Msg();//计算并输出每秒收到的网络消息
	virtual void onNetJoin(ClientSock* pClient);//只会被一个线程触发，安全
	virtual void onNetLeave(ClientSock* pClient);//有客户端离开事件
	virtual void onNetMsg(ClientSock* pClient,Header* header);
private:
	SOCKET _serverSock;
	std::vector<CellServer*> _cellServers;//消息处理对象，内部会创建线程	
	TimeStamp _tTime;
protected:
	std::atomic_int _recvCount;//收到消息计数
	std::atomic_int _clientCount;//客户端计数
};
#endif // !SERVER_H_
