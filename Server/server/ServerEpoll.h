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
	//ֻ���������¿ͻ��ˣ�����Ϊ�߳̿��ƶ��� pThread
	void OnRun(CellThread* pThread) {
		CellEpoll cellEpoll;
		cellEpoll.Create(1);//ֻ��Ҫ����һ��
		SOCKET serverSock = GetServerSock();//��ȡ������socket������
		cellEpoll.Ctrl(EPOLL_CTL_ADD, serverSock, EPOLLIN);

		//�жϸ������¿ͻ������ӵ��߳��Ƿ�������
		while (pThread->IsRun()) {
			//���㲢���ÿ���յ���������Ϣ
			Time4Msg();

			//�ȴ��¼�����
			int eventsNum = cellEpoll.Wait(1);//�ȴ�1����
			if (eventsNum < 0) {
				CELLLOG_ERROR("<socket=%d> epoll error.", (int)serverSock);
				pThread->exitThread();
				break;
			}

			//�жϷ�����socket�������Ƿ��ڼ�����
			epoll_event* events = cellEpoll.GetPEvents();//��ȡ����
			for (int i = 0; i < eventsNum; ++i) {
				//���������ɶ��¼�ʱ����ʾ���¿ͻ��˼���
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
