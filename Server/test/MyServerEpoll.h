#include"../server/ServerEpoll.h"

class MyServer :public ServerEpoll
{
public:
	MyServer()
	{
		_bSendBack = CellConfig::Instance().hasKey("-sendback");
		_bSendFull = CellConfig::Instance().hasKey("-sendfull");
		_bCheckMsgID = CellConfig::Instance().hasKey("-checkMsgID");
	}

	virtual void onNetJoin(CellClient* pClient)
	{
		Server::onNetJoin(pClient);
	}

	void onNetRecv(CellClient* pClient)
	{
		Server::onNetRecv(pClient);
	}

	virtual void onNetLeave(CellClient* pClient)//有客户端离开事件
	{
		Server::onNetLeave(pClient);
	}

	virtual void onNetMsg(CellServer* pCellServer, CellClient* pClient, netmsg_DataHeader* header)
	{
		Server::onNetMsg(pCellServer, pClient, header);
		switch (header->cmd)
		{
		case CMD_LOGIN:
		{
			//用登录消息代替心跳消息，收到客户端的登录消息相当于收到心跳消息
			pClient->ResetDTHeart();//重置心跳
			netmsg_Login* login = (netmsg_Login*)header;

			//检查消息ID
			if (_bCheckMsgID)
			{
				if (login->msgID != pClient->nRecvMsgID)
				{
					//当前消息ID和本地收消息次数不匹配
					CELLLOG_ERROR("OnNetMsg socket<%d> msgID<%d> _nRecvMsgID<%d> %d",
						pClient->GetSockfd(), login->msgID, pClient->nRecvMsgID, login->msgID - pClient->nRecvMsgID);
				}
				++pClient->nRecvMsgID;
			}

			//登录逻辑
			//......
			//回应消息
			if (_bSendBack)
			{
				netmsg_LoginResult login_result;
				login_result.msgID = pClient->nSendMsgID;
				int ret = pClient->SendData(&login_result);//发送数据
				if (ret == SOCKET_ERROR)
				{
					//发送缓冲区满了，消息没发出去,目前直接抛弃了
					//客户端消息太多，需要考虑应对策略
					//正常连接，业务客户端不会有这么多消息
					//模拟并发测试时是否发送频率过高
					if (_bSendFull)
						CELLLOG_WARRING("<Socket=%d> Send Full", pClient->GetSockfd());
				}
				else
				{
					++pClient->nSendMsgID;
				}
			}

			/*CELLLOG_INFO("netmsg_Login : socket = %d , user name = %r , password= %r",
				(int)pClient->getSockfd(), login->userName, login->passWord);*/
		}
		break;
		case CMD_LOGOUT:
		{
			pClient->ResetDTHeart();//重置心跳
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
			ss.writeArray(s, a);
			ss.writeArray(name, b);
			ss.writeArray(password, c);
			ss.finish();
			pClient->SendData(ss.getData(), ss.getWritePos());
		}
		break;
		case CMD_ERROR:
		{
			CELLLOG_ERROR("error : socket = %d , data length= %d",
				(int)pClient->GetSockfd(), header->dataLength);
		}
		break;
		default:
		{
			CELLLOG_INFO("Undefined data : socket = %d , data length=  %d",
				(int)pClient->GetSockfd(), header->dataLength);
		}
		break;
		}
	}

private:
	bool _bSendBack;//自定义标志 收到消息后将返回应答消息	
	bool _bSendFull;//自定义标志 是否提示：发送缓冲区已写满	
	bool _bCheckMsgID;//是否检查接收到的消息ID是否连续
};

