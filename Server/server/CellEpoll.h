#ifndef CELL_EPOLL_H_
#define CELL_EPOLL_H_
#ifdef __linux__

#include<sys/epoll.h>
#include"../comm/Common.h"
#include"../server/CellClient.h"

class CellEpoll {
public:
	~CellEpoll() {
		Destory();
	}

	//创建epoll描述符
	int Create(int nMaxEvents) {
		//避免重复创建
		if (_epollfd > 0) {
			Destory();
		}

		_epollfd = epoll_create(nMaxEvents);//创建epoll描述符
		if (_epollfd == EPOLL_ERROR) {
			CELLLOG_ERROR("CellEpoll Create error");
		}

		_pEvents = new epoll_event[nMaxEvents];
		_nMaxEvents = nMaxEvents;

		return _epollfd;
	}

	//向epoll对象注册需要管理、监听的socket文件描述符
	int Ctrl(int op, SOCKET sockfd, uint32_t events) {
		epoll_event ev;
		ev.data.fd = sockfd;//事件关联的socket描述符对象
		ev.events = events;//事件类型

		//0表示成功，-1表示失败
		int ret = epoll_ctl(_epollfd, op, sockfd, &ev);
		if (ret == EPOLL_ERROR) {
			CELLLOG_ERROR("CellEpoll Ctrl 1 error");
		}

		return ret;
	}

	//向epoll对象注册需要管理、监听的客户端
	int Ctrl(int op, CellClient* pClient, uint32_t events) {
		epoll_event ev;
		ev.data.ptr = pClient;//事件关联的客户端指针
		ev.events = events;//事件类型

		//0表示成功，-1表示失败
		int ret = epoll_ctl(_epollfd, op, pClient->GetSockfd(), &ev);
		if (ret == EPOLL_ERROR) {
			CELLLOG_ERROR("CellEpoll Ctrl 2 error");
		}

		return ret;
	}

	//等待数组中的事件发生
	int Wait(int timeout) {
		int eventsNum = epoll_wait(_epollfd, _pEvents, _nMaxEvents, timeout);//0不等待，立即返回;-1一直等待事件发生;正数：等待的时间
		if (eventsNum < 0) {
			CELLLOG_ERROR("epoll_wait,errno<%d>,errmsg<%s>", errno, strerror(errno));
		}

		return eventsNum;
	}

	//返回用于检测网络事件的数组
	epoll_event* GetPEvents() {
		return _pEvents;
	}

	//销毁
	void Destory() {
		if (_epollfd > 0) {
			CellNetwork::DestorySocket(_epollfd);
			_epollfd = -1;
		}

		if (_pEvents) {
			delete[] _pEvents;
			_pEvents = nullptr;
		}
	}

private:
	epoll_event* _pEvents = nullptr;//用于检测网络事件的数组
	int _epollfd = -1;//epoll描述符
	int _nMaxEvents;//最大连接数
};

#endif //__linux__
#endif //CELL_EPOLL_H_
