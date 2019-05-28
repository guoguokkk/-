#ifndef MY_SERVER_H_
#define MY_SERVER_H_

#include<iostream>
#include"Server.h"
#include<stdio.h>
class MyServer :public Server
{
public:
	virtual void onNetJoin(std::shared_ptr<CellClient>& pClient)//只会被一个线程触发，安全
	{
		Server::onNetJoin(pClient);
		//CellLog::Info("Client<%d> join\n", (int)pClient->GetSock());
	}

	virtual void onNetLeave(std::shared_ptr<CellClient>& pClient)//有客户端离开事件
	{
		Server::onNetLeave(pClient);
		//CellLog::Info("Client<%d> leave\n", (int)pClient->GetSock());
	}

	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<CellClient>& pClient, netmsg_Header* header)
	{
		Server::onNetMsg(pCellServer, pClient, header);
		switch (header->cmd)
		{
		case CMD_S2C_HEART:
		{
			//定义了心跳消息，但是客户端没有发送对应的心跳消息
			pClient->resetDTHeart();//重置心跳
			std::shared_ptr<netmsg_s2c_Heart> netmsg_s2c_heart = std::make_shared<netmsg_s2c_Heart>();
			pCellServer->addSendTask(pClient, (std::shared_ptr<netmsg_s2c_Heart>)netmsg_s2c_heart);
		}
		case CMD_LOGIN:
		{
			//用登录消息代替心跳消息，收到客户端的登录消息相当于收到心跳消息
			pClient->resetDTHeart();//重置心跳
			netmsg_Login* login = (netmsg_Login*)header;
			/*CellLog::Info("netmsg_Login : socket = %d , user name = %s , password= %s\n",
				(int)pClient->getSockfd(), login->userName, login->passWord);*/

			std::shared_ptr<netmsg_LoginResult> login_result = std::make_shared<netmsg_LoginResult>();
			pCellServer->addSendTask(pClient, (std::shared_ptr<netmsg_Header>)login_result);
			break;
		}

		case CMD_LOGOUT:
		{
			netmsg_Logout* logout = (netmsg_Logout*)header;
			/*CellLog::Info("netmsg_Logout : socket = %d , user name = %s ",
				(int)pClient->GetSock(), logout->userName);*/
			break;
		}

		case CMD_ERROR:
		{
			CellLog::Info("error : socket = %d , data length= %d\n",
				(int)pClient->getSockfd(), header->dataLength);
			break;
		}

		default:
		{
			CellLog::Info("Undefined data : socket = %d , data length=  %d\n",
				(int)pClient->getSockfd(), header->dataLength);
			break;
		}

		}
	}

	void onNetRecv(std::shared_ptr<CellClient>& pClient)
	{
		++_recvCount;
	}
};

#endif // !MY_SERVER_H_

