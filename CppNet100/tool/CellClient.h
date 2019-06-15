#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_
#include"Message.h"
#include<memory>
#include"../memory/ObjectPool.h"
#include"CellBuffer.h"

#define CLIENT_HEART_DEAD_TIME 60000//客户端心跳检测死亡计时，60秒
#define CLIENT_SEND_BUF_TIME 200//定时发送数据的最大时间间隔，0.2秒

//客户端数据类型，适用对象池原因：客户端频繁地退出连接
class CellClient :public ObjectPoolBase<CellClient, 1000>
{
public:
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
		CELLLOG_DEBUG("s=%d CellClient%d.~CellClient", serverId, id);
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

	//缓冲区的控制根据业务需求的差异而调整，异步发送数据
	int sendData(netmsg_Header* header)
	{
		return sendData((const char*)header, header->dataLength);
	}

	int sendData(const char* pData, int len)
	{
		//添加数据，参数为数据和数据的长度
		if (_SendBuf.push(pData, len))
		{
			return len;
		}
		return SOCKET_ERROR;
	}

	//立即将缓冲区的数据发送给客户端
	int sendDataReal()
	{
		resetDTSend();//重置发送时间
		return _SendBuf.write2socket(_sockfd);
	}

	//检测心跳	
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

	//检测数据发送的时间间隔
	bool checkSend(time_t dt)
	{
		_dtSend += dt;
		if (_dtSend >= CLIENT_SEND_BUF_TIME)
		{
			//CELLLOG_INFO("checkSend: _sockfd=%d, time=%d", (int)_sockfd, (int)_dtSend);
			//时间到了，立即将发送缓冲区的数据发送出去
			sendDataReal();

			//重置发送计时
			resetDTSend();
			return true;
		}
		return false;
	}

	SOCKET getSockfd() { return _sockfd; }
	void resetDTHeart() { _dtHeart = 0; }//重置心跳死亡计时
	void resetDTSend() { _dtSend = 0; }//重置上次发送消息的时间

	bool hasMsg() { return _recvBuf.hasMsg(); }
	int recvData() { return _recvBuf.read4socket(_sockfd); }
	netmsg_Header* front_msg() { return (netmsg_Header*)_recvBuf.data(); }
	void pop_front_msg()
	{
		if (hasMsg())
			_recvBuf.pop(front_msg()->dataLength);
	}

	//是否需要写
	bool needWrite()
	{
		return _SendBuf.needWrite();//有消息就可以写
	}

public:
	int id = -1;
	int serverId = -1;//所属server的id
private:
	SOCKET _sockfd;//客户端socket

	CellBuffer _recvBuf;//消息缓冲区
	CellBuffer _SendBuf;//发送缓冲区

	time_t _dtHeart;//心跳死亡计时
	time_t _dtSend;//上次发送消息的时间(定时发送消息)
};
#endif // !CELL_CLIENT_H_
