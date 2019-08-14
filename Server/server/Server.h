#ifndef SERVER_H_
#define SERVER_H_
#include"../comm/Common.h"
#include"../server/CellClient.h"
#include"../tool/CellNetWork.h"
#include"../server/CellFDSet.h"
#include"../server/INetEvent.h"
#include"../server/CellServer.h"
#include<vector>

class Server :public INetEvent
{
public:
	Server();
	virtual ~Server();

	SOCKET InitServer();//初始化服务器
	int Bind(const char* ip, unsigned short port);//绑定ip地址和端口
	int Listen(int n);//监听端口号		
	void CloseServer();//关闭服务器	  

	//启动CellServer，参数为CellServer的数量，ServerType为服务器类型(使用select或者epoll)
	template<class ServerType>
	void StartAllCellServer(int nCellServer)
	{
		//启动四个线程来负责消息处理业务
		for (int i = 0; i < nCellServer; ++i)
		{
			auto cellServer = new ServerType();//新建一个服务器来负责消息处理业务，参数为id
			cellServer->SetId(i + 1);
			_cellServers.push_back(cellServer);
			cellServer->SetEventObj(this);//注册网络事件接受对象
			cellServer->StartCellServer();//启动消息处理线程
		}

		//启动线程，处理新客户端连接
		_thread.startThread(nullptr,
			[this](CellThread* pThread) {
				OnRun(pThread);
			});
	}
	
	//网络事件，实现各种统计功能
	virtual void onNetJoin(CellClient* pClient);//客户端加入事件
	virtual void onNetLeave(CellClient* pClient);//客户端离开事件
	virtual void onNetMsg(CellServer* pCellServer, CellClient* pClient, netmsg_DataHeader* header);//消息统计事件
	virtual void onNetRecv(CellClient* pClient);//消息接收统计事件

protected:
	SOCKET Accept();//接受客户端连接	
	void AddClientToCellServer(CellClient* pClient);//将新客户端分配给客户数量最少的cellServer
	virtual void OnRun(CellThread* pThread) = 0;//连接新客户端
	void Time4Msg();//计算并输出每秒收到的网络消息
	SOCKET GetServerSock();//返回服务器的socket描述符

private:
	std::vector<CellServer*> _cellServers;//消息处理对象		
	CellThread _thread;//线程控制对象
	CellTimeStamp _tTime;//每秒消息计时
	SOCKET _serverSock;//服务器socket描述符

	int _nSendBufSize;//客户端发送缓冲区大小	
	int _nRecvBufSize;//客户端接收缓冲区大小	
	int _nMaxClient;//客户端连接上限

protected:
	std::atomic_int _recvCount;//收到消息计数
	std::atomic_int _clientCount;//客户端计数
	std::atomic_int _msgCount;//收到消息包计数
};
#endif // !SERVER_H_
