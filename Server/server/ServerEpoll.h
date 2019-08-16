#ifndef SERVER_EPOLL_H_
#define SERVER_EPOLL_H_
#ifdef __linux__

#include"../server/Server.h"
#include"../server/CellServerEpoll.h"

class ServerEpoll : public Server {
public:
	void StartAllCellServer(int nCellServer) {
		Server::StartAllCellServer<CellServerEpoll>(nCellServer);
	}

protected:
	//只负责连接新客户端，参数为线程控制对象 pThread
	void OnRun(CellThread* pThread) {
		CellEpoll cellEpoll;
		cellEpoll.Create(1);//只需要创建一个
		SOCKET serverSock = GetServerSock();//获取服务器socket描述符
		cellEpoll.Ctrl(EPOLL_CTL_ADD, serverSock, EPOLLIN);

		//判断负责处理新客户端连接的线程是否在运行
		while (pThread->IsRun()) {
			//计算并输出每秒收到的网络消息
			Time4Msg();

			//等待事件发生
			int eventsNum = cellEpoll.Wait(1);//等待1毫秒
			if (eventsNum < 0) {
				CELLLOG_ERROR("<socket=%d> epoll error.", (int)serverSock);
				pThread->exitThread();
				break;
			}

			//判断服务器socket描述符是否在集合中
			epoll_event* events = cellEpoll.GetPEvents();//获取集合
			for (int i = 0; i < eventsNum; ++i) {
				//当服务发生可读事件时，表示有新客户端加入
				if (events[i].data.fd == serverSock) {
					if (events[i].events & EPOLLIN) {
						Accept();
					}
				}
			}
		}
	}

};
#endif //__linux__
#endif // !SERVER_EPOLL_H_
