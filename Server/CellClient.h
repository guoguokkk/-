#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_

#include"Message.h"
#include<memory>
#include"ObjectPool.h"

#define CLIENT_HEART_DEAD_TIME 60000//客户端心跳检测死亡计时，60秒
#define CLIENT_SEND_BUF_TIME 200//定时发送数据的最大时间间隔，0.2秒

//客户端数据类型，适用对象池原因：客户端频繁地退出连接
class CellClient :public ObjectPoolBase<CellClient, 1000>
{
public:
	CellClient(SOCKET clientSock = INVALID_SOCKET);
	~CellClient();

	int sendData(std::shared_ptr<netmsg_Header> header);//发送数据，定量发送
	int sendDataDirect();//立即将缓冲区的数据发送给客户端
	void sendDataDirect(std::shared_ptr<netmsg_Header> header);//立即发送

	SOCKET getSockfd() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }
	int getLastPos() { return _lastMsgPos; }
	void setLastPos(int pos) { _lastMsgPos = pos; }

	void resetDTHeart() { _dtHeart = 0; }//重置心跳死亡计时
	void resetDTSend() { _dtSend = 0; }//重置上次发送消息的时间

	//检测心跳
	bool checkHeart(time_t dt);

	//检测数据发送的时间间隔
	bool checkSend(time_t dt);

public:
	int id = -1;
	int serverId = -1;//所属server的id
private:
	SOCKET _sockfd;//客户端socket

	char _msgBuf[RECV_BUF_SIZE];//消息缓冲区
	int _lastMsgPos;//消息缓冲区尾部位置

	char _szSendBuf[SEND_BUF_SIZE];//发送缓冲区
	int _lastSendPos;//发送缓冲区尾部位置

	time_t _dtHeart;//心跳死亡计时
	time_t _dtSend;//上次发送消息的时间(定时发送消息)
};
#endif // !CELL_CLIENT_H_
