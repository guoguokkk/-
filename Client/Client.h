#ifndef CLIENT_H_
#define CLIENT_H_
#ifdef _WIN32
#include"../Test/Message.h"
#include"../Test/TimeStamp.h"
#else
#include"Message.h"
#include"TimeStamp.h"
#endif // _WIN32
class Client {
public:
	Client(SOCKET client_sock = INVALID_SOCKET) :_client_sock(client_sock)
	{
		memset(_msg_buf, 0, sizeof(_msg_buf));
		_last_pos = 0;
		InitClient();
	}
	~Client();
	void InitClient();//��ʼ���ͻ���
	void CloseClient();//�رտͻ���
	int Connect(const char* ip, const short port);//���ӷ�����
	bool OnRun();//select
	int RecvMessages(SOCKET client_sock);//������Ϣ
	void HandleMessages(Header* header);//������Ϣ
	void Send(Header* header);//������Ϣ
private:
	SOCKET _client_sock;
	char _recv_buf[RECV_BUF_SIZE];
	char _msg_buf[RECV_BUF_SIZE * 10];
	int _last_pos;
};

#endif // !CLIENT_H_
