#ifndef CELL_SERVER_H_
#define CELL_SERVER_H_
#include"../tool/Common.h"
#include"../tool/CellFDSet.h"
#include"../tool/CellTask.h"
#include"../server/CellClient.h"
#include"../server/INetEvent.h"
#include"../server/CellBuffer.h"
#include<thread>
#include<mutex>
#include<atomic>//原子操作
#include<map>
#include<vector>

//网络消息接收处理服务类
class CellServer {
public:
	CellServer(int id = -1);
	~CellServer();

	//Server启动CellServer
	void setEventObj(INetEvent* event);//绑定网络事件
	void startCellServer();//启动任务执行和消息处理业务线程

	//Server向CellServer添加客户数据
	void addClient(std::shared_ptr<CellClient> pClient);//把新连接到的客户端加入当前CellServer的客户端缓冲队列
	size_t getClientCount();//获取客户端总数

private:
	void closeCellServer();//关闭消息处理业务

	//消息处理业务
	void onRunCellServer(CellThread* pThread);//消息处理业务
	void checkTime();//检测心跳消息，完成定时发送数据 
	bool doSelect();//计算可读集合、可写集合，并处理
	void doMsg();//处理消息

	void readData();//处理可读集合数据-接收消息
	void writeData();//处理可写集合数据-发送消息

	int recvData(std::shared_ptr<CellClient> pClient);//触发<接收到网络数据>事件
	virtual void onNetMsg(std::shared_ptr<CellClient>& pClient, netmsg_DataHeader* header);//处理网络消息
	void onClientLeave(std::shared_ptr<CellClient> pClient);//客户端离开
	void clearClients();//清理正式客户队列和缓冲客户队列

	//没有使用任务执行类，只用了一个线程来完成消息的处理
	//void addSendTask(std::shared_ptr<CellClient> pClient, netmsg_DataHeader* header);//添加发送任务
private:
	std::map<SOCKET, std::shared_ptr<CellClient>> _clients;//正式客户队列，map
	std::vector<std::shared_ptr<CellClient>> _clientsBuf;//缓冲客户队列，vector
	std::mutex _mutex;//缓冲队列的锁
	INetEvent* _pNetEvent;//网络事件对象

	CellFDSet _fdRead;
	CellFDSet _fdWrite;
	CellFDSet _fdReadBak;//客户列表备份
	bool _clientsChange = true;//客户列表是否改变

	CellTaskServer _taskServer;//执行任务类	
	SOCKET _maxSock;
	time_t _oldTime = CellTime::getNowInMillSec();//旧时间戳

	int _id = -1;
	CellThread _thread;//线程控制类
};

#endif // !CELL_SERVER_H_