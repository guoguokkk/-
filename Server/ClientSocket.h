#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H
#include"../Test/Common.h"
class ClientSocket {
public:
	ClientSocket(int sockfd = INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_msg_buf, 0, sizeof(_msg_buf));//清空消息缓冲区
		_last_pos = 0;
	}
	int GetSockfd() const { return _sockfd; }
	char* GetMsgBuf() { return _msg_buf; }
	int GetLastPos() const { return _last_pos; }
	void SetLastPos(int pos) { _last_pos = pos; }
private:
	int _sockfd;//文件描述符
	char _msg_buf[RECV_BUF_SIZE * 10];//消息缓冲区
	int _last_pos = 0;//上次存放的位置（消息缓冲区）
};

#endif // !CLIENT_SOCKET_H
