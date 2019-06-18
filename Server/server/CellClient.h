#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_
#include<memory>
#include"../tool/Common.h"
#include"../server/CellBuffer.h"
#include"../memory/ObjectPool.h"

//客户端，适用对象池原因：客户端频繁地退出连接
class CellClient :public ObjectPoolBase<CellClient, 10050>
//class CellClient
{
	//////////用于调试的成员变量////////////////////////////
public:
	int id = -1;//id
	int cellServerId = -1;//所属server的id

	//测试收发逻辑用	
	int nRecvMsgID = 1;//用于server检测接收到的消息ID是否连续
	int nSendMsgID = 1;//用于client检测接收到的消息ID是否连续
	/////////////////////////////////////////////////////////

public:
	//构造函数-客户端，参数为描述符 sockfd=INVALID_SOCKET，发送缓冲区大小 sendSize=SEND_BUF_SIZE，接收缓冲区大小 recvSize=RECV_BUF_SIZE
	CellClient(SOCKET sockfd = INVALID_SOCKET, int sendSize = SEND_BUF_SIZE, int recvSize = RECV_BUF_SIZE) :
		_SendBuf(sendSize), _recvBuf(recvSize)
	{
		static int n = 1;
		id = n++;
		_sockfd = sockfd;

		resetDTHeart();//心跳死亡计时初始化
		resetDTSend();//重置上次发送消息的时间
	}

	~CellClient()
	{
		CELLLOG_INFO("~CELLClient[sId=%d id=%d socket=%d]", cellServerId, id, (int)_sockfd);
		if (_sockfd != INVALID_SOCKET)
		{
#ifdef _WIN32
			closesocket(_sockfd);
#else
			close(_sockfd);
#endif // _WIN32
		}
		_sockfd = INVALID_SOCKET;
	}

	//获取客户端的套接字描述符
	SOCKET getSockfd() { return _sockfd; }

	//接收数据，返回值为消息长度
	int recvData() { return _recvBuf.read4socket(_sockfd); }

	//接收到的数据是否有至少一条消息
	bool hasMsg() { return _recvBuf.hasMsg(); }

	//获取接收到的第一条消息
	netmsg_DataHeader* front_msg() { return (netmsg_DataHeader*)_recvBuf.getData(); }

	//是否需要写入发送缓冲区，判断依据：缓冲区有消息就需要写并发送
	bool needWrite() { return _SendBuf.needWrite(); }

	//弹出第一条消息
	void pop_front_msg()
	{
		if (hasMsg())
			_recvBuf.pop(front_msg()->dataLength);
	}

	//立即将缓冲区的数据发送给客户端
	int sendDataReal()
	{
		resetDTSend();//重置发送时间
		return _SendBuf.write2socket(_sockfd);
	}

	//缓冲区的控制根据业务需求的差异而调整，异步发送数据
	int sendData(netmsg_DataHeader* header)
	{
		return sendData((const char*)header, header->dataLength);
	}

	//发送数据
	int sendData(const char* pData, int len)
	{
		//添加数据，参数为数据和数据的长度
		if (_SendBuf.push(pData, len))
		{
			return len;
		}
		return SOCKET_ERROR;
	}	

	//重置心跳死亡计时
	void resetDTHeart() { _dtHeart = 0; }

	//重置上次发送消息的时间
	void resetDTSend() { _dtSend = 0; }

	//检测心跳，参数为时间 dt
	bool checkHeart(time_t dt)
	{
		_dtHeart += dt;
		if (_dtHeart >= CLIENT_HEART_DEAD_TIME)
		{
			CELLLOG_INFO("checkHeart dead:s=%d,time=%ld", _sockfd, _dtHeart);
			return true;
		}
		return false;
	}

	//检测数据发送的时间间隔，参数为时间 dt
	bool checkSend(time_t dt)
	{
		_dtSend += dt;
		if (_dtSend >= CLIENT_SEND_BUF_TIME)
		{			
			sendDataReal();//时间到了，立即将发送缓冲区的数据发送出去
			resetDTSend();//重置发送计时
			return true;
		}
		return false;
	}

private:
	SOCKET _sockfd;//客户端描述符

	CellBuffer _recvBuf;//消息缓冲区
	CellBuffer _SendBuf;//发送缓冲区

	time_t _dtHeart;//心跳死亡计时
	time_t _dtSend;//上次发送消息的时间(定时发送消息)		
	int _sendBuffFullCount = 0;//发送缓冲区遇到写满情况计数
};
#endif // !CELL_CLIENT_H_
