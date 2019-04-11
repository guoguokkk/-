#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H
#ifdef _WIN32
#include"../Test/Common.h"
#else
#include"Common.h"
#endif // _WIN32
class ClientSocket {
public:
	ClientSocket(int sockfd = INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_msg_buf, 0, sizeof(_msg_buf));
		_last_pos = 0;
	}
	int GetSockfd() const { return _sockfd; }
	char* GetMsgBuf() { return _msg_buf; }
	int GetLastPos() const { return _last_pos; }
	void SetLastPos(int pos) { _last_pos = pos; }
private:
	int _sockfd;
	char _msg_buf[RECV_BUF_SIZE * 10];
	int _last_pos = 0;
};

#endif // !CLIENT_SOCKET_H
