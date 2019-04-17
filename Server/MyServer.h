#ifndef MY_SERVER_H_
#define MY_SERVER_H_
#include<iostream>
#include"Server.h"
#include<stdio.h>
class MyServer :public Server
{
public:
	virtual void OnNetJoin(ClientSock* pClient)//只会被一个线程触发，安全
	{
		++_client_count;
		//printf("Client<%d> join\n", (int)pClient->GetSock());
	}

	virtual void OnNetLeave(ClientSock* pClient)//有客户端离开事件
	{
		--_client_count;
		//printf("Client<%d> leave\n", (int)pClient->GetSock());
	}

	virtual void OnNetMsg(ClientSock* pClient, Header* header)//有客户端离开时
	{
		++_recv_count;
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			Login* login = (Login*)header;
			/*printf("Login : socket = %d , user name = %s , password= %s\n",
				(int)pClient->GetSock(), login->userName, login->passWord);*/

			LoginResult login_result;
			pClient->SendData(&login_result);
		}
		break;
		case CMD_LOGOUT:
		{
			Logout* logout = (Logout*)header;
			/*printf("Logout : socket = %d , user name = %s ",
				(int)pClient->GetSock(), logout->userName);*/

			LogoutResult logout_result;
			pClient->SendData(&logout_result);
		}
		break;
		case CMD_ERROR:
		{
			printf("error : socket = %d , data length= %d\n",
				(int)pClient->GetSock(), header->data_length);
		}
		break;
		default:
		{
			printf("Undefined data : socket = %d , data length=  %d\n",
				(int)pClient->GetSock(), header->data_length);
		}
		break;
		}
	}
};


#endif // !MY_SERVER_H_

