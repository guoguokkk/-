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
	void InitClient();//初始化客户端
	void CloseClient();//关闭客户端
	int Connect(const char* ip, const short port);//连接服务器
	bool OnRun();//select
	int RecvMessages(SOCKET client_sock);//接收消息
	void HandleMessages(Header* header);//处理消息
	void Send(Header* header);//发送消息
private:
	SOCKET _client_sock;
	char _recv_buf[RECV_BUF_SIZE];
	char _msg_buf[RECV_BUF_SIZE * 10];
	int _last_pos;
};

#endif // !CLIENT_H_
