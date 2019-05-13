#ifndef MY_SERVER_H_
#define MY_SERVER_H_

#include<iostream>
#include"Server.h"
#include<stdio.h>
class MyServer :public Server
{
public:
	virtual void onNetJoin(std::shared_ptr<ClientSock>& pClient)//只会被一个线程触发，安全
	{
		Server::onNetJoin(pClient);
		//printf("Client<%d> join\n", (int)pClient->GetSock());
	}

	virtual void onNetLeave(std::shared_ptr<ClientSock>& pClient)//有客户端离开事件
	{
		Server::onNetLeave(pClient);
		//printf("Client<%d> leave\n", (int)pClient->GetSock());
	}

	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<ClientSock>& pClient, Header* header)
	{
		Server::onNetMsg(pCellServer, pClient, header);
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login* login = (Login*)header;
			/*printf("Login : socket = %d , user name = %s , password= %s\n",
				(int)pClient->getSock(), login->userName, login->passWord);*/

			std::shared_ptr<LoginResult> login_result = std::make_shared<LoginResult>();
			pCellServer->addSendTask(pClient, (std::shared_ptr<Header>)login_result);
			break;
		}

		case CMD_LOGOUT:
		{
			Logout* logout = (Logout*)header;
			/*printf("Logout : socket = %d , user name = %s ",
				(int)pClient->GetSock(), logout->userName);*/
			break;
		}

		case CMD_ERROR:
		{
			printf("error : socket = %d , data length= %d\n",
				(int)pClient->getSock(), header->data_length);
			break;
		}

		default:
		{
			printf("Undefined data : socket = %d , data length=  %d\n",
				(int)pClient->getSock(), header->data_length);
			break;
		}

		}
	}

	void onNetRecv(std::shared_ptr<ClientSock>& pClient)
	{
		++_recvCount;
	}
};

#endif // !MY_SERVER_H_

