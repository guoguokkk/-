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
	void InitClient();//初始化客户端
	int Connect(const char* ip, const short port);//连接服务器
	void CloseClient();//关闭客户端	
	bool OnRun();//处理网络消息
	int RecvData(SOCKET client_sock);//接收消息
	void OnNetMsg(Header* header);//处理消息
	int SendData(Header* header, int len);//发送消息
private:
	SOCKET _client_sock;
	char _recv_buf[RECV_BUF_SIZE];
	char _msg_buf[RECV_BUF_SIZE * 5];
	int _last_pos;
	bool _is_connect;
};

#endif // !CLIENT_H_
