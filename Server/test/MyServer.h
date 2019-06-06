#ifndef MY_SERVER_H_
#define MY_SERVER_H_
#include"../server/Server.h"
#include<iostream>
#include<stdio.h>
#include"../tool/CellMsgStream.h"
#include<stdint.h>

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
			netmsg_s2c_Heart* netmsg_s2c_heart = new netmsg_s2c_Heart();
			pCellServer->addSendTask(pClient, (netmsg_s2c_Heart*)netmsg_s2c_heart);
		}
		case CMD_LOGIN:
		{
			//用登录消息代替心跳消息，收到客户端的登录消息相当于收到心跳消息
			pClient->resetDTHeart();//重置心跳
			netmsg_Login* login = (netmsg_Login*)header;
			/*CellLog::Info("netmsg_Login : socket = %d , user name = %r , password= %r\n",
				(int)pClient->getSockfd(), login->userName, login->passWord);*/

			netmsg_LoginResult* login_result = new netmsg_LoginResult();
			pCellServer->addSendTask(pClient, (netmsg_Header*)login_result);
			break;
		}

		case CMD_LOGOUT:
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

			CellWriteStream ss(128);
			ss.setNetCmd(CMD_LOGOUT_RESULT);
			ss.writeInt8(n1);
			ss.writeInt16(n2);
			ss.writeInt32(n3);
			ss.writeFloat(n4);
			ss.writeDouble(n5);
			ss.writeArray(s,a);
			ss.writeArray(name,b);
			ss.writeArray(password,c);
			ss.finish();
			//pCellServer->addSendTask(pClient, (netmsg_Header*)ss.getData());
			pClient->sendData(ss.getData(),ss.getWritePos());


				/*	netmsg_Logout* logout = (netmsg_Logout*)header;
					CellLog::Info("netmsg_Logout : socket = %d , user name = %s \n",
						(int)pClient->getSockfd(), logout->userName);*/
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

