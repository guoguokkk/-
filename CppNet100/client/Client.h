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
	virtual void onNetMsg(netmsg_DataHeader* header);//处理消息
	int sendData(netmsg_DataHeader* header);//发送消息
	int sendData(const char* pData,int len);//发送消息
	bool isRun();
	void initClient(int sendSize = SEND_BUF_SIZE, int recvSize = RECV_BUF_SIZE);//初始化客户端

private:
	bool _isConnect;

protected:
	CellClient* _pClient = nullptr;
};

#endif // !CLIENT_H_
