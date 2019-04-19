#ifndef MY_SERVER_H_
#define MY_SERVER_H_
#include<iostream>
#include"Server.h"
#include<stdio.h>
class MyServer :public Server
{
public:
	virtual void onNetJoin(ClientSock* pClient)//ֻ�ᱻһ���̴߳�������ȫ
	{
		++_clientCount;
		//printf("Client<%d> join\n", (int)pClient->GetSock());
	}

	virtual void onNetLeave(ClientSock* pClient)//�пͻ����뿪�¼�
	{
		--_clientCount;
		//printf("Client<%d> leave\n", (int)pClient->GetSock());
	}

	virtual void onNetMsg(ClientSock* pClient, Header* header)//�пͻ����뿪ʱ
	{
		++_msgCount;
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login* login = (Login*)header;
			/*printf("Login : socket = %d , user name = %s , password= %s\n",
				(int)pClient->GetSock(), login->userName, login->passWord);*/

			//LoginResult login_result;
			//pClient->sendData(&login_result);
			break;
		}

		case CMD_LOGOUT:
		{
			Logout* logout = (Logout*)header;
			/*printf("Logout : socket = %d , user name = %s ",
				(int)pClient->GetSock(), logout->userName);*/

			LogoutResult logout_result;
			pClient->sendData(&logout_result);
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

	void onNetRecv(ClientSock* pClient)
	{
		++_recvCount;
	}
};

#endif // !MY_SERVER_H_

