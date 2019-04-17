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
		_server_sock = INVALID_SOCKET;
		_recv_count = 0;//!一定要初始化
		_client_count = 0;//!一定要初始化
	}
	~Server();
	SOCKET InitServer();//初始化服务器
	int Bind(const char* ip, const short port);//绑定ip地址和端口
	int Listen(int n);//监听客户端
	SOCKET Accept();//接收客户端
	void AddClientToCellServer(ClientSock* p_client);
	void StartServer(int n_cell_server);
	void CloseServer();//关闭服务器	   	 
	bool OnRun();//select
	void Time4Msg();//计算并输出每秒收到的网络消息
	virtual void OnNetJoin(ClientSock* p_client);//只会被一个线程触发，安全
	virtual void OnNetLeave(ClientSock* p_client);//有客户端离开事件
	virtual void OnNetMsg(ClientSock* p_client,Header* header);
private:
	SOCKET _server_sock;
	std::vector<CellServer*> _cell_servers;//消息处理对象，内部会创建线程	
	TimeStamp _tTime;
protected:
	std::atomic_int _recv_count;//收到消息计数
	std::atomic_int _client_count;//客户端计数
};
#endif // !SERVER_H_
