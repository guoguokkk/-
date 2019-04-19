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
	virtual ~Client();
	void initClient();//初始化客户端
	int connectToServer(const char* ip, unsigned short port);//连接服务器
	void closeClient();//关闭客户端	
	bool onRun();//处理网络消息
	bool isRun();
	int recvData(SOCKET clientSock);//接收消息
	virtual void onNetMsg(Header* header);//处理消息
	int sendData(Header* header, int nLen);//发送消息
private:
	SOCKET _clientSock;
	bool _isConnect;
	char _msgBuf[RECV_BUF_SIZE] = {};
	int _lastMsgPos = 0;
};

#endif // !CLIENT_H_
