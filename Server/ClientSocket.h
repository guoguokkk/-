#ifndef CLIENT_SOCKET_H
#define CLIENT_SOCKET_H
#include"../Test/Common.h"
class ClientSocket {
public:
	ClientSocket(int sockfd = INVALID_SOCKET)
	{
		_sockfd = sockfd;
		memset(_msg_buf, 0, sizeof(_msg_buf));//�����Ϣ������
		_last_pos = 0;
	}
	int GetSockfd() const { return _sockfd; }
	char* GetMsgBuf() { return _msg_buf; }
	int GetLastPos() const { return _last_pos; }
	void SetLastPos(int pos) { _last_pos = pos; }
private:
	int _sockfd;//�ļ�������
	char _msg_buf[RECV_BUF_SIZE * 10];//��Ϣ������
	int _last_pos = 0;//�ϴδ�ŵ�λ�ã���Ϣ��������
};

#endif // !CLIENT_SOCKET_H
