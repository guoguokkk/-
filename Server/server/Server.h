#ifndef SERVER_H_
#define SERVER_H_
#include"../tool/Common.h"
#include"../server/CellClient.h"
#include"../tool/CellNetWork.h"
#include"../tool/CellFDSet.h"
#include"../server/INetEvent.h"
#include"../server/CellServer.h"
#include<vector>

class Server :public INetEvent
{
public:
	Server();
	virtual ~Server();

	SOCKET initServer();//初始化
	int Bind(const char* ip, unsigned short port);//绑定ip地址和端口
	int Listen(int n);//监听端口号
	void startAllCellServer(int nCellServer);//启动所有的消息处理CellServer线程和新客户端连接线程
	void closeServer();//关闭	    

	virtual void onNetJoin(std::shared_ptr<CellClient> pClient);//客户端加入事件
	virtual void onNetLeave(std::shared_ptr<CellClient> pClient);//客户端离开事件
	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<CellClient> pClient, netmsg_DataHeader* header);//消息统计事件
	virtual void onNetRecv(std::shared_ptr<CellClient> pClient);//消息接收统计事件

private:
	SOCKET Accept();//接受客户端连接	
	void addClientToCellServer(std::shared_ptr<CellClient> pClient);//将新客户端分配给客户数量最少的cellServer
	void onRun(CellThread* pThread);//连接新客户端
	void time4Msg();//计算并输出每秒收到的网络消息

private:
	SOCKET _serverSock;
	std::vector<CellServer*> _cellServers;//消息处理对象，内部会创建线程	
	CellTimeStamp _tTime;//每秒消息计时
	CellThread _thread;//线程控制对象

	int _nSendBufSize;//客户端发送缓冲区大小	
	int _nRecvBufSize;//客户端接收缓冲区大小	
	int _nMaxClient;//客户端连接上限

protected:
	std::atomic_int _recvCount;//收到消息计数
	std::atomic_int _clientCount;//客户端计数
	std::atomic_int _msgCount;//收到消息包计数
};
#endif // !SERVER_H_
