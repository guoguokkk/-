#ifndef CLIENT_H_
#define CLIENT_H_

#include"../tool/Message.h"
#include"../tool/CellTimeStamp.h"
#include"../tool/CellClient.h"

class Client {
public:
	Client();
	virtual ~Client();

	int connectToServer(const char* ip, unsigned short port);//连接服务器
	void closeClient();//关闭客户端	
	bool onRun();//处理网络消息	
	int recvData(SOCKET clientSock);//接收消息
	virtual void onNetMsg(netmsg_Header* header) = 0;//处理消息
	int sendData(netmsg_Header* header, int nLen);//发送消息

private:
	bool isRun();
	void initClient();//初始化客户端

private:
	bool _isConnect;

protected:
	CellClient* _pClient = nullptr;
};

#endif // !CLIENT_H_
