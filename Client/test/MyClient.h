#ifndef MYCLIENT_H_
#define MYCLIENT_H_
#include"../client/Client.h"
#include"../tool/CellStream.h"
#include"../tool/CellMsgStream.h"
class MyClient :public Client
{
public:
	MyClient() {}
	~MyClient() {}

	//处理消息
	void onNetMsg(netmsg_Header* header)
	{
		switch (header->cmd)
		{
		case CMD_LOGIN_RESULT:
		{
			netmsg_LoginResult* login_result = (netmsg_LoginResult*)header;
			/*CellLog::Info("netmsg_Login result : socket = %d , data length= %d , result= %d\n",
				(int)_clientSock, login_result->dataLength, login_result->result);*/
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			netmsg_LogoutResult* logout_result = (netmsg_LogoutResult*)header;
			/*CellLog::Info("netmsg_Logout result : socket = %d , data length= %d , result= %d\n",
				(int)_client_sock, logout_result->dataLength, logout_result->result);*/
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			netmsg_NewUserJoin* new_user_join = (netmsg_NewUserJoin*)header;
			/*CellLog::Info("New user join : socket = %d , data length = %d, sock= %d\n",
				(int)_client_sock, new_user_join->dataLength, new_user_join->sock);*/
		}
		break;
		case CMD_ERROR:
		{
			CellLog::Info("error : socket = %d , data length= %d\n",
				(int)_pClient->getSockfd(), header->dataLength);
		}
		break;
		default:
		{
			CellLog::Info("Undefined data : socket = %d , data length=  %d\n",
				(int)_pClient->getSockfd(), header->dataLength);
		}
		break;
		}
	}
private:

};

#endif // !MYCLIENT_H_

