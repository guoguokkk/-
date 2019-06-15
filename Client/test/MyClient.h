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
			/*CELLLOG_INFO("netmsg_Login result : socket = %d , data length= %d , result= %d\n",
				(int)_clientSock, login_result->dataLength, login_result->result);*/
		}
		break;
		case CMD_LOGOUT_RESULT:
		{
			CellReadStream r(header);
			int8_t n1;
			r.readInt8(n1);
			int16_t n2;
			r.readInt16(n2);
			int32_t n3;
			r.readInt32(n3);
			float n4;
			r.readFloat(n4);
			double n5;
			r.readDouble(n5);
			char s[20] = {};
			int a = r.readArray(s, 20);//返回的是数组元素个数
			char name[20] = {};
			int b = r.readArray(name, 20);//返回的是数组元素个数
			int password[20] = {};
			int c = r.readArray(password, 20);//返回的是数组元素个数

			netmsg_LogoutResult* logout_result = (netmsg_LogoutResult*)header;
			/*CELLLOG_INFO("netmsg_Logout result : socket = %d , data length= %d , result= %d\n",
				(int)_client_sock, logout_result->dataLength, logout_result->result);*/
		}
		break;
		case CMD_NEW_USER_JOIN:
		{
			netmsg_NewUserJoin* new_user_join = (netmsg_NewUserJoin*)header;
			/*CELLLOG_INFO("New user join : socket = %d , data length = %d, sock= %d\n",
				(int)_client_sock, new_user_join->dataLength, new_user_join->sock);*/
		}
		break;
		case CMD_ERROR:
		{
			CELLLOG_INFO("error : socket = %d , data length= %d\n",
				(int)_pClient->getSockfd(), header->dataLength);
		}
		break;
		default:
		{
			CELLLOG_INFO("Undefined data : socket = %d , data length=  %d\n",
				(int)_pClient->getSockfd(), header->dataLength);
		}
		break;
		}
	}
private:

};

#endif // !MYCLIENT_H_

