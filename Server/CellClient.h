#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_

#include"Message.h"
#include<memory>
#include"ObjectPool.h"
#include"CellBuffer.h"

#define CLIENT_HEART_DEAD_TIME 60000//客户端心跳检测死亡计时，60秒
#define CLIENT_SEND_BUF_TIME 200//定时发送数据的最大时间间隔，0.2秒

//客户端数据类型，适用对象池原因：客户端频繁地退出连接
class CellClient :public ObjectPoolBase<CellClient, 1000>
{
public:
	CellClient(SOCKET clientSock = INVALID_SOCKET);
	~CellClient();

	int sendDataAsynchronous(std::shared_ptr<netmsg_Header> header);//异步发送数据
	int sendData(std::shared_ptr<netmsg_Header> header);//发送数据，定量发送
	int sendDataReal();//立即将缓冲区的数据发送给客户端

	bool checkHeart(time_t dt);//检测心跳	
	bool checkSend(time_t dt);//检测数据发送的时间间隔

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

public:
	int id = -1;
	int serverId = -1;//所属server的id
private:
	SOCKET _sockfd;//客户端socket

	CellBuffer _recvBuf;//消息缓冲区
	CellBuffer _SendBuf;//发送缓冲区

	time_t _dtHeart;//心跳死亡计时
	time_t _dtSend;//上次发送消息的时间(定时发送消息)

	int _sendBufFullCount = 0;//发送缓冲区写满的次数
};
#endif // !CELL_CLIENT_H_
