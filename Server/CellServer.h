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

//消息处理
class CellServer {
public:
	CellServer(SOCKET server_sock);
	~CellServer();	
	void SetEventObj(INetEvent* event) { _pEvent = event; }
	void CloseServer();//关闭服务器
	bool OnRun();//select	
	int RecvData(ClientSock* pClient);//接收消息，处理粘包、少包
	void OnNetMsg(ClientSock* pClient, Header* header);//响应网络数据
	void AddClient(ClientSock* pClient);//增加客户端
	void StartCellServer();
	size_t GetClientCount();
private:
	SOCKET _server_sock;
	std::vector<ClientSock*> _clients;//正式客户队列
	std::vector<ClientSock*> _clientsBuf;//缓冲客户队列
	char _recv_buf[RECV_BUF_SIZE];//接收缓冲区
	std::mutex _mutex;//缓冲队列的锁
	std::thread* _pThread;
	INetEvent* _pEvent;//网络事件对象
};

#endif // !CELL_SERVER
