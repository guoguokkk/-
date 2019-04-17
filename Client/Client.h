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
	Client();
	~Client();
	void InitClient();//��ʼ���ͻ���
	int Connect(const char* ip, const short port);//���ӷ�����
	void CloseClient();//�رտͻ���	
	bool OnRun();//����������Ϣ
	int RecvData(SOCKET client_sock);//������Ϣ
	void OnNetMsg(Header* header);//������Ϣ
	int SendData(Header* header, int len);//������Ϣ
private:
	SOCKET _client_sock;
	char _recv_buf[RECV_BUF_SIZE];
	char _msg_buf[RECV_BUF_SIZE * 5];
	int _last_pos;
	bool _is_connect;
};

#endif // !CLIENT_H_
