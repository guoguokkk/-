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
	SendMsgToClientTask(std::shared_ptr<ClientSock> pClient, std::shared_ptr<Header> header)
	{
		_pClient = pClient;
		_pHeader = header;
	}

	//执行任务，发送消息
	void doTask()
	{
		_pClient->sendData(_pHeader);
	}
private:
	std::shared_ptr<ClientSock> _pClient;//目标客户端
	std::shared_ptr<Header> _pHeader;//要发送的数据
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
	int recvData(std::shared_ptr<ClientSock> pClient);//接收消息，处理粘包、少包
	virtual void onNetMsg(std::shared_ptr<ClientSock>& pClient, Header* header);//响应网络数据
	void addClient(std::shared_ptr<ClientSock> pClient);//增加客户端
	void startCellServer();
	size_t getClientCount();
	void addSendTask(std::shared_ptr<ClientSock> pClient, std::shared_ptr<Header> header)
	{
		auto task = std::make_shared<SendMsgToClientTask>(pClient, header);
		_taskServer.addTask((std::shared_ptr<Task>)task);
	}
private:
	SOCKET _serverSock;
	std::map<SOCKET, std::shared_ptr<ClientSock>> _clients;//正式客户队列
	std::vector<std::shared_ptr<ClientSock>> _clientsBuf;//缓冲客户队列
	std::mutex _mutex;//缓冲队列的锁
	std::thread _thread;
	INetEvent* _pEvent;//网络事件对象
	fd_set _fdReadBack;//客户列表备份
	bool _clientsChange;//客户列表是否改变
	SOCKET _maxSock;
	TaskServer _taskServer;
};

#endif // !CELL_SERVER_
