#ifndef CELL_CLIENT_H_
#define CELL_CLIENT_H_

#include"Message.h"
#include<memory>
#include"ObjectPool.h"

#define CLIENT_HEART_DEAD_TIME 5000//客户端心跳检测死亡计时，5000毫秒

//客户端数据类型，适用对象池原因：客户端频繁地退出连接
class CellClient :public ObjectPoolBase<CellClient, 10000>
{
public:
	CellClient(SOCKET clientSock = INVALID_SOCKET);
	~CellClient();

	int sendData(std::shared_ptr<netmsg_Header> header);//发送数据，定时定量发送

	SOCKET getSockfd() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }
	int getLastPos() { return _lastMsgPos; }
	void setLastPos(int pos) { _lastMsgPos = pos; }

	void resetDTHeart() { _dtHeart = 0; }//重置心跳死亡计时

	//检测心跳
	bool checkHeart(time_t dt)
	{
		_dtHeart += dt;
		if (_dtHeart >= CLIENT_HEART_DEAD_TIME)
		{
			printf("checkHeart dead: _sockfd=%d, time=%d\n", (int)_sockfd, (int)_dtHeart);
			return true;
		}
		return false;
	}
private:
	SOCKET _sockfd;//客户端socket

	char _msgBuf[RECV_BUF_SIZE];//消息缓冲区
	int _lastMsgPos;//消息缓冲区尾部位置

	char _sendBuf[SEND_BUF_SIZE];//发送缓冲区
	int _lastSendPos;//发送缓冲区尾部位置

	time_t _dtHeart;//心跳死亡计时
};
#endif // !CELL_CLIENT_H_
