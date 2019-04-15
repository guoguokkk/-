#ifndef CLIENT_SOCK_H_
#define CLIENT_SOCK_H_
#ifdef _WIN32
#include"../Test/Message.h"
#else
#include"Message.h"
#endif // _WIN32
//�ͻ�����
class ClientSock {
public:
	ClientSock(SOCKET client_sock = INVALID_SOCKET) :_client_sock(client_sock)
	{
		memset(_msg_buf, 0, sizeof(_msg_buf));
		_last_pos = 0;
	}
	~ClientSock() = default;
	void SetLastPos(int new_last_pos) { _last_pos = new_last_pos; }
	int GetLastPos() { return _last_pos; }
	char* GetMsgBuf() { return _msg_buf; }
	SOCKET GetSock() { return _client_sock; }
private:
	char _msg_buf[RECV_BUF_SIZE * 10];//��Ϣ��������һ��Ҫ���ڽ��ջ�����
	int _last_pos;//��Ϣ���������һ��λ��
	SOCKET _client_sock;//�ͻ���socket
};
#endif // !CLIENT_SOCK_H_
