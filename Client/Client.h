#ifndef CLIENT_H_
#define CLIENT_H_

#include"Message.h"
#include"TimeStamp.h"

class Client {
public:
	Client();
	virtual ~Client();

	int connectToServer(const char* ip, unsigned short port);//连接服务器
	void closeClient();//关闭客户端	
	bool onRun();//处理网络消息	
	int recvData(SOCKET clientSock);//接收消息
	virtual void onNetMsg(netmsg_Header* header);//处理消息
	int sendData(netmsg_Header* header, int nLen);//发送消息

private:
	bool isRun();
	void initClient();//初始化客户端

private:
	SOCKET _clientSock;
	bool _isConnect;
	char _msgBuf[RECV_BUF_SIZE] = {};
	int _lastMsgPos = 0;
};

#endif // !CLIENT_H_
