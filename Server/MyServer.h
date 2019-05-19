#ifndef MY_SERVER_H_
#define MY_SERVER_H_

#include<iostream>
#include"Server.h"
#include<stdio.h>
class MyServer :public Server
{
public:
	virtual void onNetJoin(std::shared_ptr<CellClient>& pClient)//ֻ�ᱻһ���̴߳�������ȫ
	{
		Server::onNetJoin(pClient);
		//printf("Client<%d> join\n", (int)pClient->GetSock());
	}

	virtual void onNetLeave(std::shared_ptr<CellClient>& pClient)//�пͻ����뿪�¼�
	{
		Server::onNetLeave(pClient);
		//printf("Client<%d> leave\n", (int)pClient->GetSock());
	}

	virtual void onNetMsg(CellServer* pCellServer, std::shared_ptr<CellClient>& pClient, netmsg_Header* header)
	{
		Server::onNetMsg(pCellServer, pClient, header);
		switch (header->cmd)
		{
		case CMD_S2C_HEART:
		{
			//������������Ϣ�����ǿͻ���û�з��Ͷ�Ӧ��������Ϣ
			pClient->resetDTHeart();//��������
			std::shared_ptr<netmsg_s2c_Heart> netmsg_s2c_heart = std::make_shared<netmsg_s2c_Heart>();
			pCellServer->addSendTask(pClient, (std::shared_ptr<netmsg_s2c_Heart>)netmsg_s2c_heart);
		}
		case CMD_LOGIN:
		{
			//�õ�¼��Ϣ����������Ϣ���յ��ͻ��˵ĵ�¼��Ϣ�൱���յ�������Ϣ
			pClient->resetDTHeart();//��������
			netmsg_Login* login = (netmsg_Login*)header;
			/*printf("netmsg_Login : socket = %d , user name = %s , password= %s\n",
				(int)pClient->getSockfd(), login->userName, login->passWord);*/

			std::shared_ptr<netmsg_LoginResult> login_result = std::make_shared<netmsg_LoginResult>();
			pCellServer->addSendTask(pClient, (std::shared_ptr<netmsg_Header>)login_result);
			break;
		}

		case CMD_LOGOUT:
		{
			netmsg_Logout* logout = (netmsg_Logout*)header;
			/*printf("netmsg_Logout : socket = %d , user name = %s ",
				(int)pClient->GetSock(), logout->userName);*/
			break;
		}

		case CMD_ERROR:
		{
			printf("error : socket = %d , data length= %d\n",
				(int)pClient->getSockfd(), header->dataLength);
			break;
		}

		default:
		{
			printf("Undefined data : socket = %d , data length=  %d\n",
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

