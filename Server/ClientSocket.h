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
	ClientSock(SOCKET client_sock = INVALID_SOCKET) :_client_sock(client_sock)
	{
		memset(_msg_buf, 0, sizeof(_msg_buf));//初始化消息缓冲区
		_last_pos = 0;
	}
	~ClientSock() = default;
	void SetLastPos(int pos) { _last_pos = pos; }
	int GetLastPos() { return _last_pos; }
	char* GetMsgBuf() { return _msg_buf; }
	SOCKET GetSock() { return _client_sock; }

	//发送数据
	int SendData(Header* header)
	{
		if (header)
		{
			//return send(_client_sock, (char*) &header, header->data_length, 0);//!!!画蛇添足，一直不能识别数据
			return send(_client_sock, (char*) header, header->data_length, 0);
		}
		return SOCKET_ERROR;
	}
private:
	char _msg_buf[RECV_BUF_SIZE * 10];//消息缓冲区，!!!一定要大于接收缓冲区
	int _last_pos;//消息缓冲区最后一个位置
	SOCKET _client_sock;//客户端socket
};
#endif // !CLIENT_SOCK_H_
