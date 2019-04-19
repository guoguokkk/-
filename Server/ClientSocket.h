#ifndef CLIENT_SOCK_H_
#define CLIENT_SOCK_H_

#ifdef _WIN32
#include"../Test/Message.h"
#else
#include"Message.h"
#endif // _WIN32

//客户端数据类型
class ClientSock {
public:
	ClientSock(SOCKET clientSock = INVALID_SOCKET)
	{
		_sockfd = clientSock;
		memset(_msgBuf, 0, sizeof(_msgBuf));//初始化消息缓冲区
		_lastPos = 0;
	}
	SOCKET getSock() { return _sockfd; }
	char* getMsgBuf() { return _msgBuf; }	
	int getLastPos() { return _lastPos; }
	void setLastPos(int pos) { _lastPos = pos; }	

	//发送数据，定时定量发送
	int sendData(Header* header)
	{
		if (header)
		{//定时 定量
			//return send(_client_sock, (char*) &header, header->data_length, 0);//!!!画蛇添足，一直不能识别数据
			return send(_sockfd, (const char*)header, header->data_length, 0);
		}
		return SOCKET_ERROR;
	}
	
private:
	char _msgBuf[RECV_BUF_SIZE];//消息缓冲区，!!!一定要大于接收缓冲区
	int _lastPos;//消息缓冲区最后一个位置
	SOCKET _sockfd;//客户端socket
	char _sendBuf[SEND_BUF_SIZE];//发送缓冲区
};
#endif // !CLIENT_SOCK_H_
