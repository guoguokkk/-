#ifndef CELL_SERVER_H_
#define CELL_SERVER_H_

#include"Message.h"
#include<vector>
#include"CellClient.h"
#include"INetEvent.h"
#include<thread>
#include<mutex>
#include<atomic>//原子操作
#include<map>
#include"CellTask.h"

class INetEvent;

//网络消息发送队列
class SendMsgToClientTask
{
public:
	SendMsgToClientTask(std::shared_ptr<CellClient> pClient, std::shared_ptr<Header> header)
	{
		_pClient = pClient;
		_pHeader = header;
	}
private:
	std::shared_ptr<CellClient> _pClient;//目标客户端
	std::shared_ptr<Header> _pHeader;//要发送的数据
};

//消息处理类
class CellServer {
public:
	CellServer(SOCKET serverSock = INVALID_SOCKET);
	~CellServer();

	void setEventObj(INetEvent* event);//绑定网络事件	
	void addClient(std::shared_ptr<CellClient> pClient);//增加客户端
	void startCellServer();
	size_t getClientCount();
	void addSendTask(std::shared_ptr<CellClient> pClient, std::shared_ptr<Header> header)
	{
		auto task = std::make_shared<SendMsgToClientTask>(pClient, header);

		//执行任务
		_taskServer.addTask([pClient, header]() {
			pClient->sendData(header);
			});
	}

private:
	void closeServer();//关闭服务器
	bool onRun();//select	
	bool isRun();//判断服务器是否在运行
	int recvData(std::shared_ptr<CellClient> pClient);//接收消息，处理粘包、少包
	virtual void onNetMsg(std::shared_ptr<CellClient>& pClient, Header* header);//响应网络数据

private:
	SOCKET _serverSock;
	std::map<SOCKET, std::shared_ptr<CellClient>> _clients;//正式客户队列
	std::vector<std::shared_ptr<CellClient>> _clientsBuf;//缓冲客户队列
	std::mutex _mutex;//缓冲队列的锁
	std::thread _thread;
	INetEvent* _pEvent;//网络事件对象
	fd_set _fdReadBack;//客户列表备份
	bool _clientsChange;//客户列表是否改变
	SOCKET _maxSock;
	TaskServer _taskServer;
};

#endif // !CELL_SERVER_H_
