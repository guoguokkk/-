#ifndef CELL_SERVER_EPOLL_H_
#define CELL_SERVER_EPOLL_H_
#ifdef __linux__

#include"../server/CellServer.h"
#include"../server/CellEpoll.h"

//epoll-网络消息接收处理服务类
class CellServerEpoll : public CellServer {
public:
	//跨线程时，父类中有纯虚函数，子类中实现了，子类先释放，父类还没有释放，父类仍有可能执行纯虚函数，报错-需要虚析构函数
	~CellServerEpoll() noexcept {
		CloseCellServer();
	}

	CellServerEpoll() {
		_cellEpoll.Create(10240);
	}

	//epoll-
	bool DoNetEvents() {
		/////////////////////////////////////////////////////////
		//有客户端需要写入，加入可写事件
		for (auto client : _clients) {
			//需要向客户端发送数据,才加入fd_set检测是否可写
			if (client.second->NeedWrite()) {
				_cellEpoll.Ctrl(EPOLL_CTL_MOD, client.second, EPOLLOUT);
			}
			else {
				_cellEpoll.Ctrl(EPOLL_CTL_MOD, client.second, EPOLLIN);
			}
		}

		/////////////////////////////////////////////////////////
		//等待事件发生
		int eventsNum = _cellEpoll.Wait(1);
		if (eventsNum < 0) {//出错
			return false;
		}
		else if (eventsNum == 0) {//没有事件发生
			return true;
		}

		/////////////////////////////////////////////////////////
		//有事件发生
		epoll_event* events = _cellEpoll.GetPEvents();//用于检测事件是否发生的数组
		for (int i = 0; i < eventsNum; ++i) {
			CellClient* pClient = (CellClient*)events[i].data.ptr;

			//客户端发生事件
			if (pClient) {

				//该客户端发生可读事件
				if (events[i].events & EPOLLIN) {
					int ret = RecvData(pClient);//接收消息
					if (ret == SOCKET_ERROR) {
						RmClient(pClient);
						continue;
					}
				}

				//该客户端发生可写事件
				if (events[i].events & EPOLLOUT) {
					int ret = pClient->SendDataReal();//发送消息
					if (ret == SOCKET_ERROR) {
						RmClient(pClient);
					}
				}
			}
		}

		return true;
	}

	//收发数据出错，移除客户端
	void RmClient(CellClient* pClient) {
		//在客户端队列中删除该客户端
		auto iter = _clients.find(pClient->GetSockfd());
		if (iter != _clients.end())
			_clients.erase(iter);

		//客户端离开，更新统计数据和客户端列表是否变化的标志_clientsChange
		OnClientLeave(pClient);
	}

	//新客户端加入
	virtual void OnClientJoin(CellClient* pClient) {
		_cellEpoll.Ctrl(EPOLL_CTL_ADD, pClient, EPOLLIN);
	}

private:
	CellEpoll _cellEpoll;
	SOCKET _maxSock;
};

#endif //__linux__
#endif // !CELL_SERVER_EPOLL_H_
