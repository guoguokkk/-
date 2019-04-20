#ifndef CELL_SERVER_
#define CELL_SERVER_

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
#include"Task.h"



//网络消息发送队列
class SendMsgToClientTask :public Task
{
public:
	SendMsgToClientTask(ClientSock* pClient, Header* header)
	{
		_pClient = pClient;
		_pHeader = header;
	}

	//执行任务，发送消息
	void doTask()
	{
		_pClient->sendData(_pHeader);
		delete _pHeader;
	}
private:
	ClientSock* _pClient;//目标客户端
	Header* _pHeader;//要发送的数据
};

class INetEvent;

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
	void addSendTask(ClientSock* pClient, Header* header)
	{
		SendMsgToClientTask* task = new SendMsgToClientTask(pClient, header);
		_taskServer.addTask(task);
	}
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
	TaskServer _taskServer;
};

#endif // !CELL_SERVER_
