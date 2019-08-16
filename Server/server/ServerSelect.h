#ifndef SERVER_SELECT_H_
#define SERVER_SELECT_H_

#include"../server/Server.h"
#include"../server/CellServerSelect.h"

class ServerSelect : public Server {
public:
	void StartAllCellServer(int nCellServer)
	{
		Server::StartAllCellServer<CellServerSelect>(nCellServer);
	}

protected:
	//ֻ���������¿ͻ��ˣ�����Ϊ�߳̿��ƶ��� pThread
	void OnRun(CellThread* pThread)
	{
		CellFDSet fd_read;//�ɶ�����

		//�жϸ������¿ͻ������ӵ��߳��Ƿ�������
		while (pThread->IsRun())
		{
			Time4Msg();//���㲢���ÿ���յ���������Ϣ

			SOCKET serverSock = GetServerSock();//��ȡ������socket������
			fd_read.Zero();//����ɶ�����
			fd_read.Add(serverSock); //��������������������ɶ�����

			timeval time;
			time.tv_sec = 0;//��
			time.tv_usec = 1;
			int ret = select(serverSock + 1, fd_read.GetFdSet(), nullptr, nullptr, &time);
			if (ret < 0)
			{
				CELLLOG_ERROR("<socket=%d> select error.", (int)serverSock);
				pThread->exitThread();
				break;
			}
			//���������������ڿɶ������У��������¿ͻ���
			if (fd_read.Has(serverSock))
			{
				Accept();
			}
		}
	}
};

#endif // !SERVER_SELECT_H_
