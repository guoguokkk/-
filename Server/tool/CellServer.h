#ifndef CELL_SERVER_H_
#define CELL_SERVER_H_

#include"CellTask.h"
#include<thread>
#include<mutex>
#include<atomic>//原子操作
#include<map>
#include<vector>
#include"Message.h"
#include"CellClient.h"
#include"../server/INetEvent.h"
#include"CellTimeStamp.h"
#include"CellSemaphore.h"
#include"CellThread.h"
#include"CellBuffer.h"

//网络消息发送队列
class SendMsgToClientTask
{
public:
	SendMsgToClientTask(std::shared_ptr<CellClient> pClient, netmsg_Header* header)
	{
		_pClient = pClient;
		_pHeader = header;
	}
private:
	std::shared_ptr<CellClient> _pClient;//目标客户端
	netmsg_Header* _pHeader;//要发送的数据
};


class INetEvent;

//消息处理类
class CellServer {
public:
	CellServer(int id = -1);
	~CellServer();

	void setEventObj(INetEvent* event);//绑定网络事件	
	void addClient(std::shared_ptr<CellClient> pClient);//增加客户端
	void startCellServer();//启动工作线程	
	size_t getClientCount();
	void addSendTask(std::shared_ptr<CellClient> pClient, netmsg_Header* header);
	
private:
	void closeServer();//关闭服务器
	void onRunCellServer(CellThread* pThread);//select	
	int recvData(std::shared_ptr<CellClient> pClient);//接收消息，处理粘包、少包
	virtual void onNetMsg(std::shared_ptr<CellClient>& pClient, netmsg_Header* header);//响应网络数据

	void readData(fd_set& fd_read);//处理数据
	void writeData(fd_set& fd_write);

	void checkTime();//检测心跳消息，完成定时发送数据 
	void clearClients();

	void onClientLeave(std::shared_ptr<CellClient> pClient);

private:
	//大的往前放，小的往后放，内存对齐
	std::map<SOCKET, std::shared_ptr<CellClient>> _clients;//正式客户队列
	std::vector<std::shared_ptr<CellClient>> _clientsBuf;//缓冲客户队列

	std::mutex _mutex;//缓冲队列的锁
	INetEvent* _pNetEvent;//网络事件对象

	CellTaskServer _taskServer;
	fd_set _fdReadBack;//客户列表备份
	bool _clientsChange;//客户列表是否改变
	SOCKET _maxSock;
	time_t _oldTime = CellTime::getNowInMillSec();//旧时间戳
	
	int _id;
	CellThread _thread;
};

#endif // !CELL_SERVER_H_