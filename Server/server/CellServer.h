#ifndef CELL_SERVER_H_
#define CELL_SERVER_H_

#include"../comm/Common.h"
#include"../server/CellFDSet.h"
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
	virtual ~CellServer();

	void CloseCellServer();//关闭消息处理业务
	void SetId(int id);//设置id

	//Server启动CellServer
	void SetEventObj(INetEvent* event);//绑定网络事件
	void StartCellServer();//启动任务执行和消息处理业务线程

	//Server向CellServer添加客户数据
	void AddClient(CellClient* pClient);//把新连接到的客户端加入当前CellServer的客户端缓冲队列
	size_t GetClientCount();//获取客户端总数
	int RecvData(CellClient* pClient);//触发<接收到网络数据>事件
	void OnClientLeave(CellClient* pClient);//客户端离开

protected:
	//消息处理业务
	void OnRunCellServer(CellThread* pThread);//消息处理业务
	void CheckTime();//检测心跳消息，完成定时发送数据
	void DoMsg();//处理消息
	virtual bool DoNetEvents() = 0;//slect/epoll
	virtual void onNetMsg(CellClient* pClient, netmsg_DataHeader* header);//处理网络消息

	virtual void OnClientJoin(CellClient* pClient);//新客户端加入
	void ClearClients();//清理正式客户队列和缓冲客户队列

	//没有使用任务执行类，只用了一个线程来完成消息的处理
	//void addSendTask(CellClient* pClient, netmsg_DataHeader* header);//添加发送任务
private:
	std::vector<CellClient*> _clientsBuf;//缓冲客户队列，vector
	std::mutex _mutex;//缓冲队列的锁
	INetEvent* _pNetEvent = nullptr;//网络事件对象
	CellTaskServer _taskServer;//执行任务类

	time_t _oldTime = CellTime::getNowInMillSec();//旧时间戳
	CellThread _thread;//线程控制类

protected:
	int _id = -1;
	bool _clientsChange = true;//客户列表是否改变
	std::map<SOCKET, CellClient*> _clients;//正式客户队列，map
};

#endif // !CELL_SERVER_H_
