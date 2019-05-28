#ifndef SERVER_H_
#define SERVER_H_

#include"Message.h"
#include"TimeStamp.h"
#include"INetEvent.h"
#include"CellClient.h"
#include"CellServer.h"
#include<vector>
#include"CellThread.h"

class Server :public INetEvent
{
public:
	Server();	
	~Server();
	SOCKET initServer();//初始化服务器
	int Bind(const char* ip, unsigned short port);//绑定ip地址和端口
	int Listen(int n);//监听客户端
	SOCKET Accept();//接收客户端
	void addClientToCellServer(std::shared_ptr<CellClient> pClient);
	void startServer(int n_cellServer);
	void closeServer();//关闭服务器	   	 
	
	void time4Msg();//计算并输出每秒收到的网络消息
	virtual void onNetJoin(std::shared_ptr<CellClient> pClient);//只会被一个线程触发，安全
	virtual void onNetLeave(std::shared_ptr<CellClient> pClient);//有客户端离开事件
	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<CellClient> pClient, netmsg_Header* header);
	virtual void onNetRecv(std::shared_ptr<CellClient> pClient);
private:
	void onRun(CellThread* pThread);//select

private:
	SOCKET _serverSock;
	std::vector<CellServer*> _cellServers;//消息处理对象，内部会创建线程	
	CellTimeStamp _tTime;
	CellThread _thread;
protected:
	std::atomic_int _recvCount;//收到消息计数
	std::atomic_int _clientCount;//客户端计数
	std::atomic_int _msgCount;//收到消息包计数
};
#endif // !SERVER_H_
