#ifndef CELL_SERVER
#define CELL_SERVER

#ifdef _WIN32
#include"../Test/Message.h"
#else
#include"Message.h"
#endif // _WIN32

#include<vector>
#include"ClientSocket.h"
#include"INetEvent.h"
#include<thread>
#include<mutex>
#include<atomic>//原子操作
#include<map>
//消息处理
class CellServer {
public:
	CellServer(SOCKET serverSock = INVALID_SOCKET)
	{
		_serverSock = serverSock;
		_pEvent = nullptr;
	}
	~CellServer();
	void setEventObj(INetEvent* event);
	void closeServer();//关闭服务器
	bool onRun();//select	
	bool isRun();
	int recvData(ClientSock* pClient);//接收消息，处理粘包、少包
	virtual void onNetMsg(ClientSock* pClient, Header* header);//响应网络数据
	void addClient(ClientSock* pClient);//增加客户端
	void startCellServer();
	size_t getClientCount();
private:
	SOCKET _serverSock;
	std::map<SOCKET, ClientSock*> _clients;//正式客户队列
	std::vector<ClientSock*> _clientsBuf;//缓冲客户队列
	std::mutex _mutex;//缓冲队列的锁
	std::thread _thread;
	INetEvent* _pEvent;//网络事件对象
	fd_set _fdReadBack;//客户列表备份
	bool _clientsChange;//客户列表是否改变
	SOCKET _maxSock;
};

#endif // !CELL_SERVER
