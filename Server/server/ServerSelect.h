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
	//只负责连接新客户端，参数为线程控制对象 pThread
	void OnRun(CellThread* pThread)
	{
		CellFDSet fd_read;//可读集合

		//判断负责处理新客户端连接的线程是否在运行
		while (pThread->IsRun())
		{
			Time4Msg();//计算并输出每秒收到的网络消息

			SOCKET serverSock = GetServerSock();//获取服务器socket描述符
			fd_read.Zero();//清理可读集合
			fd_read.Add(serverSock); //将服务器端描述符加入可读集合

			timeval time;
			time.tv_sec = 0;//秒
			time.tv_usec = 1;
			int ret = select(serverSock + 1, fd_read.GetFdSet(), nullptr, nullptr, &time);
			if (ret < 0)
			{
				CELLLOG_ERROR("<socket=%d> select error.", (int)serverSock);
				pThread->exitThread();
				break;
			}
			//服务器端描述符在可读集合中，则连接新客户端
			if (fd_read.Has(serverSock))
			{
				Accept();
			}
		}
	}
};

#endif // !SERVER_SELECT_H_
